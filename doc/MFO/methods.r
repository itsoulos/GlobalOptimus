library(tidyverse)
library(readxl)
library(ggpubr)
library(scales)
library(ggplot2)

# Διαβάζουμε τα δεδομένα
dat <- read_excel("C:/Users/admin/Desktop/MFO/tbl.xlsx")

# Καθαρισμός ονομάτων/τιμών (για τυχόν κενά)
names(dat) <- trimws(names(dat))

df <- dat |>
  rename(FUNCTION = 1) |>
  pivot_longer(-FUNCTION, names_to = "Method", values_to = "Error") |>
  mutate(
    Method = trimws(as.character(Method)),
    FUNCTION = as.factor(FUNCTION)
  ) |>
  filter(!is.na(Error) & Error >= 0 & Error <= 100000)

# Τα επίπεδα των μεθόδων με συγκεκριμένη σειρά (ώστε να υπάρχει GENRBF αν υπάρχει στο αρχείο)
preferred_levels <- c("PROPOSED","GENETIC","WOA","MEMOA","DE","IPSO","AOA","GENRBF", "ACO")
present_levels <- intersect(preferred_levels, unique(df$Method))
df <- df |> mutate(Method = factor(Method, levels = present_levels))

# 7 ζεύγη συγκρίσεων (κρατάμε μόνο όσα υπάρχουν στα δεδομένα)
cmp_all <- list(
  c("PROPOSED", "GENETIC"),
  c("PROPOSED", "WOA"),
  c("PROPOSED", "MEMOA"),
  c("PROPOSED", "DE"),
  c("PROPOSED", "IPSO"),
  c("PROPOSED", "AOA"),
  c("PROPOSED", "GENRBF"),
  c("PROPOSED", "ACO")
)
cmp <- Filter(function(p) all(p %in% levels(df$Method)), cmp_all)

# --- MANUAL ύψη για ΚΑΘΕ σύγκριση, στην ίδια σειρά με το cmp ---
# Βάλε αρκετό headroom ώστε να φαίνεται και το PROPOSED vs GENRBF
label_y <- c(35000, 38000, 41000, 44000, 47000, 50000, 53000, 56000)[seq_along(cmp)]
y_top   <- max(60000, max(label_y) + 4)

# Αποφυγή σφάλματος "Insufficient values in manual scale" (όσα σχήματα όσες και οι μέθοδοι)
n_methods  <- length(levels(df$Method))
shape_pool <- c(15,16,17,18,19,20,21,0,1,2,3,4,7,8,9,10)
shape_vals <- shape_pool[seq_len(n_methods)]

# --- ΝΕΟ: Υπολογισμός 95% CI & effect size (rank-biserial) για καθε ζεύγος, + Holm correction ---
# Παράγουμε vector διαφορών (PROPOSED - OTHER) ανά dataset και κάνουμε one-sample Wilcoxon πάνω στις διαφορές
pair_stats <- lapply(cmp, function(pair) {
  m1 <- pair[1]; m2 <- pair[2]
  wide <- df |>
    filter(Method %in% c(m1, m2)) |>
    select(FUNCTION, Method, Error) |>
    pivot_wider(names_from = Method, values_from = Error)
  if (!all(c(m1, m2) %in% names(wide))) return(NULL)
  d <- wide[[m1]] - wide[[m2]]
  d <- d[!is.na(d)]
  d_nz <- d[d != 0]                      # αφαιρούμε τα μηδενικά για σταθερότητα στο exact/CI
  if (length(d_nz) < 1) return(NULL)
  wt <- suppressWarnings(wilcox.test(d_nz, mu = 0, conf.int = TRUE, exact = FALSE))
  n  <- length(d_nz)
  V  <- unname(wt$statistic)
  r_rb <- (2*V)/(n*(n+1)) - 1            # rank-biserial effect size για Wilcoxon signed-rank
  tibble(
    comparison = paste(m1, "vs", m2),
    n = n,
    V = V,
    r_rank_biserial = r_rb,
    conf.low = wt$conf.int[1],
    conf.high = wt$conf.int[2],
    p = wt$p.value
  )
})
stats_tbl <- bind_rows(pair_stats) |>
  mutate(
    p_adj = p.adjust(p, method = "holm"),
    p_signif = cut(p_adj,
                   breaks = c(-Inf, 1e-4, 1e-3, 1e-2, 5e-2, Inf),
                   labels = c("****","***","**","*","ns"))
  )

# Αποθήκευση αναλυτικού πίνακα για αναπαραγωγιμότητα
out_csv <- "C:/Users/admin/Desktop/MFO/wilcoxon_CI_effectsize_table2.csv"
if (nrow(stats_tbl) > 0) readr::write_csv(stats_tbl, out_csv)

# Γράφημα
set.seed(123)
p <- ggboxplot(df, x = "Method", y = "Error",
          color = "Method", shape = "Method",
          add = "jitter",
          palette = "jco", width = 0.8,
          add.params = list(size = 1.5)) +
  stat_compare_means(
    paired = TRUE,
    group.by = "FUNCTION",               # paired ανά dataset
    method = "wilcox.test",
    method.args = list(exact = FALSE),  # αποφεύγει warning με μηδενικά/δεσμούς
    p.adjust.method = "holm",           # ΔΙΟΡΘΩΣΗ ΠΟΛΛΑΠΛΩΝ ΣΥΓΚΡΙΣΕΩΝ (Reviewer 2)
    comparisons = cmp,
    label = "p.signif",
    label.y = label_y,                  # MANUAL ύψη: εμφανίζονται ΟΛΕΣ (συμπεριλαμβανομένου GENRBF)
    size = 4
  ) +
  annotate("text",
           x = 2.8,
           y = y_top - 2,
           label = "Overall: paired Wilcoxon. Holm-adjusted p.",
           size = 4, color = "black", hjust = 0.5) +
  labs(x = "", y = "Function calls",
       title = "Method Comparison",
       subtitle = "Pairwise Wilcoxon tests") +
  theme_minimal(base_size = 15) +
  scale_y_continuous(labels = comma, limits = c(0, y_top)) +
  scale_shape_manual(values = shape_vals) +
  theme(
    legend.position = "none",
    axis.line = element_line(color = "blue", linewidth = 0.4),
    axis.ticks = element_line(color = "black"),
    axis.text.x = element_text(color = "blue", angle = 35, hjust = 1, vjust = 1),  # οριζόντιος άξονας σε γωνία
    axis.text.y = element_text(color = "blue"),                                     # κατακόρυφος κανονικά
    panel.grid.major = element_blank(),  # χωρίς πλέγμα (Reviewer 3)
    panel.grid.minor = element_blank()
  ) +
  annotate("text", x = 5.6, y = y_top,
           label = "ns: p > 0.05  (Not significant)\n****: p < 0.0001 (Very extremely significant)",
           hjust = 0, size = 2, color = "blue") +
  coord_cartesian(clip = "off")

print(p)

# (Προαιρετικά) Αποθήκευση του γραφήματος σε αρχείο υψηλής ανάλυσης
ggsave("C:/Users/admin/Desktop/RBF_THREEPHASES/table2_boxplot.png",
       plot = p, width = 10, height = 6, dpi = 300)

# (Προαιρετικά) Καταγραφή εκδόσεων για αναπαραγωγιμότητα
# sessionInfo()
