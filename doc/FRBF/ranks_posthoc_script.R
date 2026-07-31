# =====================================================================
# SCRIPT: Average Ranks + Post-hoc Πίνακες (Comment 2 του κριτή)
# =====================================================================
# Καλύπτει:
#   (α) Average ranks ανά μέθοδο, για classification (Table 2) και
#       regression (Table 3) datasets.
#   (β) Post-hoc συγκρίσεις (Conover test, Holm correction) σε μορφή
#       ΠΙΝΑΚΑ (για το κείμενο), όχι μόνο σε εικόνα.
#   (γ) Rank-based summary για τα regression datasets, ως εναλλακτικό
#       του raw-average AVERAGE (λύνει και το πρόβλημα κλίμακας που
#       αναφέρει ο κριτής: STOCK ~180 vs AIRFOIL ~0.005).
#   (δ) SD / variability στις 30 επαναλήψεις: ΧΡΕΙΑΖΕΤΑΙ raw per-run
#       δεδομένα (όχι μόνο τους μέσους όρους που έχεις στα tbl2.xlsx /
#       tbl3.xlsx). Βλ. ενότητα 5 παρακάτω με οδηγίες + template.
# =====================================================================

# ================== PACKAGES ==================
library(tidyverse)
library(readxl)
library(scales)

if (!requireNamespace("PMCMRplus", quietly = TRUE)) install.packages("PMCMRplus")
library(PMCMRplus)

if (!requireNamespace("gt", quietly = TRUE)) install.packages("gt")
library(gt)

if (!requireNamespace("writexl", quietly = TRUE)) install.packages("writexl")
library(writexl)

# ================== OUTPUT DIRECTORY ==================
# ΟΡΙΣΕ ΕΔΩ ΤΟΝ ΦΑΚΕΛΟ ΟΠΟΥ ΘΕΛΕΙΣ ΝΑ ΑΠΟΘΗΚΕΥΤΟΥΝ ΤΑ ΑΡΧΕΙΑ ΕΞΟΔΟΥ.
# Αν δεν υπάρχει, θα δημιουργηθεί αυτόματα.
output_dir <- "C:/Users/admin/Desktop/FRBF/results"
if (!dir.exists(output_dir)) dir.create(output_dir, recursive = TRUE)
setwd(output_dir)
cat(sprintf(">>> Τα αρχεία εξόδου θα αποθηκευτούν στο: %s\n", getwd()))

# ================== HELPERS ==================

# Μορφοποίηση p-value σε απλό string (για πίνακες, όχι plotmath)
# --- Εσωτερική συνάρτηση για ΕΝΑ p-value ---
.format_p_scalar <- function(p) {
  if (is.na(p)) return("NA")
  if (p < 2.2e-16) return("<2.2e-16")
  if (p < 1e-3) return(formatC(p, format = "e", digits = 2))
  formatC(p, format = "f", digits = 4)
}
# --- Vectorized wrapper: δουλεύει είτε σε ένα p-value είτε σε vector/στήλη ---
format_p <- function(p) {
  vapply(p, .format_p_scalar, character(1))
}

# Συνάρτηση που διαβάζει ένα αρχείο τύπου tbl2.xlsx / tbl3.xlsx
# (πρώτη στήλη = DATASET, οι υπόλοιπες = μέθοδοι, τιμές = μέσο σφάλμα
# από τα 30 runs) και επιστρέφει καθαρισμένο long-format data frame.
read_results <- function(path, error_min = 0, error_max = Inf) {
  dat <- read_excel(path)
  dat |>
    rename(DATASET = 1) |>
    pivot_longer(-DATASET, names_to = "Method", values_to = "Error") |>
    mutate(Method = toupper(trimws(Method))) |>
    filter(!is.na(Error), Error >= error_min, Error <= error_max)
}

# Συνάρτηση που υπολογίζει average ranks + Friedman + post-hoc (Conover/Holm)
# df_long: long format με στήλες DATASET, Method, Error
# desired_levels: επιθυμητή σειρά μεθόδων
analyze_ranks <- function(df_long, desired_levels, target_method = "PROPOSED") {

  present_methods <- desired_levels[desired_levels %in% unique(df_long$Method)]
  if (length(present_methods) < 2) stop("Χρειάζονται τουλάχιστον 2 μέθοδοι.")

  df1 <- df_long |>
    filter(Method %in% present_methods) |>
    group_by(DATASET, Method) |>
    summarise(Error = mean(Error, na.rm = TRUE), .groups = "drop")

  # Κρατάμε μόνο datasets όπου υπάρχουν ΟΛΕΣ οι μέθοδοι (απαραίτητο για Friedman)
  common_datasets <- df1 |>
    group_by(DATASET) |>
    summarise(nm = n_distinct(Method), .groups = "drop") |>
    filter(nm == length(present_methods)) |>
    pull(DATASET)

  common_df <- df1 |>
    filter(DATASET %in% common_datasets) |>
    as.data.frame()

  common_df$Method  <- factor(common_df$Method, levels = present_methods)
  common_df$DATASET <- factor(common_df$DATASET)

  # ---------- (α) AVERAGE RANKS ----------
  # Rank 1 = καλύτερη (μικρότερο σφάλμα) ανά dataset
  rank_df <- common_df |>
    group_by(DATASET) |>
    mutate(Rank = rank(Error, ties.method = "average")) |>
    ungroup()

  avg_ranks <- rank_df |>
    group_by(Method) |>
    summarise(
      Mean_Error = mean(Error),
      SD_across_datasets = sd(Error),   # μεταβλητότητα ΜΕΤΑΞΥ datasets (όχι μεταξύ 30 runs)
      Avg_Rank = mean(Rank),
      .groups = "drop"
    ) |>
    arrange(Avg_Rank)

  # ---------- (β) FRIEDMAN TEST ----------
  fr <- suppressWarnings(friedman.test(Error ~ Method | DATASET, data = common_df))

  # ---------- (γ) POST-HOC (CONOVER + HOLM) ----------
  ph <- try(PMCMRplus::frdAllPairsConoverTest(Error ~ Method | DATASET, data = common_df),
            silent = TRUE)
  if (inherits(ph, "try-error")) {
    warning("Conover post-hoc failed; falling back to Nemenyi.")
    ph <- PMCMRplus::frdAllPairsNemenyiTest(Error ~ Method | DATASET, data = common_df)
  }
  p_mat <- ph$p.value

  get_pair_p <- function(a, b, P) {
    rns <- rownames(P); cns <- colnames(P)
    if (a %in% rns && b %in% cns) return(P[a, b])
    if (b %in% rns && a %in% cns) return(P[b, a])
    NA_real_
  }

  if (target_method %in% present_methods) {
    others <- setdiff(present_methods, target_method)
    pairs_tbl <- tibble(group1 = target_method, group2 = others)
  } else {
    pairs_tbl <- t(combn(present_methods, 2)) |>
      as_tibble(.name_repair = "minimal") |>
      setNames(c("group1", "group2"))
  }

  posthoc_tbl <- pairs_tbl |>
    mutate(
      p_raw = map2_dbl(group1, group2, ~ get_pair_p(.x, .y, p_mat)),
      p_adj_holm = p.adjust(p_raw, method = "holm"),
      Significant = ifelse(p_adj_holm < 0.05, "Yes", "No")
    )

  list(
    present_methods = present_methods,
    n_common_datasets = length(common_datasets),
    common_datasets = common_datasets,
    avg_ranks = avg_ranks,
    friedman_stat = fr$statistic,
    friedman_p = fr$p.value,
    posthoc = posthoc_tbl,
    rank_df = rank_df
  )
}

# Βοηθητική: μορφοποιημένη εκτύπωση αποτελεσμάτων στην κονσόλα + gt table
print_results <- function(res, title) {
  cat("\n====================================================\n")
  cat(title, "\n")
  cat("====================================================\n")
  cat(sprintf("Common datasets used: %d\n", res$n_common_datasets))
  cat(sprintf("Friedman chi-squared = %.3f, p = %s\n",
              res$friedman_stat, format_p(res$friedman_p)))
  cat("\n--- Average Ranks (χαμηλότερο = καλύτερο) ---\n")
  print(res$avg_ranks |>
          mutate(Mean_Error = round(Mean_Error, 3),
                 SD_across_datasets = round(SD_across_datasets, 3),
                 Avg_Rank = round(Avg_Rank, 2)))
  cat("\n--- Post-hoc (Conover test, Holm-corrected) ---\n")
  print(res$posthoc |>
          mutate(p_raw = format_p(p_raw),
                 p_adj_holm = format_p(p_adj_holm)))
}

# =====================================================================
# 1) CLASSIFICATION (Table 2) — ΠΡΟΣΑΡΜΟΣΕ ΤΟ PATH
# =====================================================================
path_classification <- "C:/Users/admin/Desktop/FRBF/tbl2.xlsx"
desired_levels_cls <- c("ADAM", "BFGS", "NEAT", "RBF", "GRBF", "PROPOSED")

df_cls <- read_results(path_classification, error_min = 0, error_max = 100)
res_cls <- analyze_ranks(df_cls, desired_levels_cls, target_method = "PROPOSED")
print_results(res_cls, "CLASSIFICATION — Average Ranks & Post-hoc")

# Export πίνακα (XLSX, έτοιμο για ενσωμάτωση στο άρθρο ως Table)
write_xlsx(res_cls$avg_ranks, "avg_ranks_classification.xlsx")
write_xlsx(res_cls$posthoc |>
             mutate(p_raw = format_p(p_raw), p_adj_holm = format_p(p_adj_holm)),
           "posthoc_classification.xlsx")

# Ωραιοποιημένος πίνακας (gt) — μπορεί να εξαχθεί ως εικόνα/HTML/Word
gt_cls <- res_cls$avg_ranks |>
  mutate(Mean_Error = round(Mean_Error, 2),
         Avg_Rank = round(Avg_Rank, 2)) |>
  select(Method, Mean_Error, Avg_Rank) |>
  gt() |>
  tab_header(title = "Average Classification Error and Friedman Ranks") |>
  cols_label(Method = "Method", Mean_Error = "Mean Error [%]",
             Avg_Rank = "Average Rank") |>
  tab_source_note(sprintf("Overall Friedman test: chi-sq = %.2f, p = %s (N = %d datasets)",
                           res_cls$friedman_stat, format_p(res_cls$friedman_p),
                           res_cls$n_common_datasets))
print(gt_cls)
gtsave(gt_cls, "avg_ranks_classification.png")

# =====================================================================
# 2) REGRESSION (Table 3) — ΠΡΟΣΑΡΜΟΣΕ ΤΟ PATH
# =====================================================================
path_regression <- "C:/Users/admin/Desktop/FRBF/tbl3.xlsx"
desired_levels_reg <- c("ADAM", "BFGS", "NEAT", "RBF", "GRBF", "PROPOSED")

df_reg <- read_results(path_regression, error_min = 0, error_max = Inf)
res_reg <- analyze_ranks(df_reg, desired_levels_reg, target_method = "PROPOSED")
print_results(res_reg, "REGRESSION — Average Ranks & Post-hoc")

write_xlsx(res_reg$avg_ranks, "avg_ranks_regression.xlsx")
write_xlsx(res_reg$posthoc |>
             mutate(p_raw = format_p(p_raw), p_adj_holm = format_p(p_adj_holm)),
           "posthoc_regression.xlsx")

gt_reg <- res_reg$avg_ranks |>
  mutate(Mean_Error = round(Mean_Error, 3),
         Avg_Rank = round(Avg_Rank, 2)) |>
  select(Method, Mean_Error, Avg_Rank) |>
  gt() |>
  tab_header(title = "Average Regression Error and Friedman Ranks") |>
  cols_label(Method = "Method", Mean_Error = "Mean Error (raw scale)",
             Avg_Rank = "Average Rank") |>
  tab_source_note(sprintf("Overall Friedman test: chi-sq = %.2f, p = %s (N = %d datasets)",
                           res_reg$friedman_stat, format_p(res_reg$friedman_p),
                           res_reg$n_common_datasets)) |>
  tab_footnote("Mean Error is on raw scale and is dominated by high-magnitude
                datasets (e.g., STOCK, BASEBALL); Average Rank is scale-free
                and is the recommended summary metric.")
print(gt_reg)
gtsave(gt_reg, "avg_ranks_regression.png")

# =====================================================================
# 3) RANK-BASED SUMMARY ΓΙΑ REGRESSION (λύνει το πρόβλημα κλίμακας)
# =====================================================================
# Αυτό ΕΙΝΑΙ ήδη το res_reg$avg_ranks -> Avg_Rank στήλη.
# Χρησιμοποίησέ το στο κείμενο αντί για (ή μαζί με) το raw AVERAGE,
# π.χ.: "While the raw average regression error favors the Proposed
# method (3.36), this metric is dominated by high-magnitude datasets
# such as STOCK and BASEBALL. The scale-free average rank confirms
# the same ordering (Proposed = X.XX, lowest/best rank across the 25
# datasets)."

cat("\n\n>>> Χρησιμοποίησε τη στήλη Avg_Rank του res_reg ως scale-free\n")
cat(">>> εναλλακτικό του raw AVERAGE στον Πίνακα 3.\n")

# =====================================================================
# 4) ΠΡΟΑΙΡΕΤΙΚΟ: NRMSE (αν έχεις πρόσβαση στο target range κάθε dataset)
# =====================================================================
# Αν θέλεις scale-normalized error αντί για rank-based summary, χρειάζεσαι
# επιπλέον, για κάθε regression dataset, το εύρος (max-min) ή τη
# διακύμανση (variance) της μεταβλητής-στόχου y. Τότε:
#
#   NRMSE = RMSE / (y_max - y_min)     ή     NRMSE = RMSE / sd(y)
#
# Το τρέχον tbl3.xlsx περιέχει ΜΟΝΟ τα ήδη υπολογισμένα mean squared
# errors ανά μέθοδο/dataset -- ΔΕΝ περιέχει τα raw y values, οπότε αυτό
# το βήμα δεν μπορεί να τρέξει από το script αυτό. Αν έχεις πρόσβαση
# στα raw training/test sets, θα χρειαστεί ξεχωριστό υπολογισμό εκεί
# (πριν φτάσεις στο σφάλμα) -- πες μου αν τα έχεις διαθέσιμα.

# =====================================================================
# 5) SD / VARIABILITY ΣΤΙΣ 30 ΕΠΑΝΑΛΗΨΕΙΣ (Comment 2, τρίτο σκέλος)
# =====================================================================
# ΠΡΟΣΟΧΗ: Το avg_ranks$SD_across_datasets ΠΑΡΑΠΑΝΩ ΔΕΝ είναι αυτό που
# ζητά ο κριτής -- είναι η μεταβλητότητα ΑΝΑΜΕΣΑ ΣΕ DATASETS, όχι η
# μεταβλητότητα ανάμεσα στα 30 runs ΓΙΑ ΤΟ ΙΔΙΟ dataset.
#
# Το tbl2.xlsx / tbl3.xlsx που χρησιμοποιεί το script αυτό (ίδιο με το
# script_tbl2.r) περιέχει μόνο τον ΜΕΣΟ ΟΡΟ των 30 επαναλήψεων ανά
# dataset x μέθοδο -- τα raw 30 sfalmata ανά run ΔΕΝ υπάρχουν σε αυτό
# το αρχείο. Για να καλύψεις πλήρως αυτό το κομμάτι του σχολίου,
# χρειάζεσαι ένα αρχείο (π.χ. tbl2_raw_runs.xlsx) με στήλες:
#
#   DATASET | Method | Run | Error
#   ------- | ------ | --- | -----
#   APPENDICITIS | PROPOSED | 1 | 17.10
#   APPENDICITIS | PROPOSED | 2 | 18.40
#   ...           ...          ... (30 γραμμές ανά DATASET x Method)
#
# *** ΕΠΙΒΕΒΑΙΩΘΗΚΕ: δεν υπάρχει πρόσβαση στα raw per-run δεδομένα ***
# (οι 30 επαναλήψεις δεν αποθηκεύτηκαν ξεχωριστά, μόνο ο μέσος όρος
# ανά dataset x method έφτασε στα tbl2.xlsx / tbl3.xlsx).
#
# Άρα το SD-across-runs ΔΕΝ μπορεί να υπολογιστεί αναδρομικά και πρέπει
# να αναγνωριστεί ρητά ως περιορισμός στο manuscript (Limitations,
# Discussion ή Conclusions section). Προτεινόμενη διατύπωση:
#
# "One limitation of the present study is that only the mean error
#  across the 30 independent runs was retained for each dataset and
#  method combination; per-run standard deviations were not stored
#  during the original experiments and could therefore not be
#  reported. Future revisions of the experimental protocol will
#  retain per-run results to allow variability analysis alongside
#  the mean performance metrics."
#
# Ελληνικά (για δική σου αναφορά):
# "Ένας περιορισμός της παρούσας μελέτης είναι ότι διατηρήθηκε μόνο ο
#  μέσος όρος σφάλματος από τις 30 ανεξάρτητες επαναλήψεις για κάθε
#  συνδυασμό dataset-μεθόδου· οι τυπικές αποκλίσεις ανά επανάληψη δεν
#  αποθηκεύτηκαν κατά τη διεξαγωγή των πειραμάτων και συνεπώς δεν
#  μπορούν να αναφερθούν. Μελλοντικές αναθεωρήσεις του πειραματικού
#  πρωτοκόλλου θα διατηρούν τα per-run αποτελέσματα ώστε να καθίσταται
#  δυνατή η ανάλυση μεταβλητότητας παράλληλα με τις μέσες μετρικές
#  απόδοσης."
#
# ΣΗΜΕΙΩΣΗ: Η στήλη SD_across_datasets στους πίνακες avg_ranks
# (Ενότητες 1-2 παραπάνω) ΔΕΝ υποκαθιστά αυτό -- δείχνει πόσο
# διαφέρει το σφάλμα ΑΠΟ dataset σε dataset, όχι τη σταθερότητα
# της ίδιας μεθόδου σε επαναλήψεις με διαφορετικό seed. Μπορεί να
# αναφερθεί επιπρόσθετα (δείχνει robustness ανά τύπο προβλήματος),
# αλλά δεν απαντά στο ίδιο ερώτημα του κριτή.

cat("\n\n### ΣΗΜΕΙΩΣΗ ###\n")
cat("Δεν υπάρχει πρόσβαση σε raw per-run δεδομένα (30 runs).\n")
cat("Χρησιμοποίησε την προτεινόμενη διατύπωση περιορισμού (Ενότητα 5)\n")
cat("στο Limitations/Discussion section του manuscript.\n")

# =====================================================================
# 6) ΕΝΙΑΙΟ XLSX ΜΕ ΟΛΑ ΤΑ ΑΠΟΤΕΛΕΣΜΑΤΑ (πολλαπλά φύλλα)
# =====================================================================
# Πιο βολικό αν θέλεις ένα μόνο αρχείο για να επισυνάψεις/ελέγξεις.

all_results <- list(
  "AvgRanks_Classification" = res_cls$avg_ranks,
  "PostHoc_Classification"  = res_cls$posthoc |>
    mutate(p_raw = format_p(p_raw), p_adj_holm = format_p(p_adj_holm)),
  "AvgRanks_Regression" = res_reg$avg_ranks,
  "PostHoc_Regression"  = res_reg$posthoc |>
    mutate(p_raw = format_p(p_raw), p_adj_holm = format_p(p_adj_holm))
)

write_xlsx(all_results, "friedman_ranks_posthoc_ALL.xlsx")
cat("\n>>> Δημιουργήθηκε ενιαίο αρχείο: friedman_ranks_posthoc_ALL.xlsx\n")
cat(">>> (4 φύλλα: AvgRanks_Classification, PostHoc_Classification,\n")
cat(">>>  AvgRanks_Regression, PostHoc_Regression)\n")
