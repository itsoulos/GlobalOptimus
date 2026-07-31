# ================== PACKAGES ==================
library(tidyverse)
library(readxl)
library(scales)

# Post-hoc για Friedman
if (!requireNamespace("PMCMRplus", quietly = TRUE)) {
  install.packages("PMCMRplus")
}
library(PMCMRplus)

# ================== HELPERS ==================
# Plotmath label για p-values (επιστρέφει string που θα γίνει parse=TRUE) με italic(p)
format_p_label_expr <- function(p) {
  if (is.na(p)) return("italic(p) == NA")
  if (p < 2.2e-16) return("italic(p) < 2.2 %*% 10^-16")
  if (p < 1e-3) {
    e <- floor(log10(p))
    m <- p / (10^e)
    # παράσταση: italic(p) == m × 10^e  (όπου e αρνητικός τυπικά)
    return(paste0("italic(p) == ", formatC(m, format = "f", digits = 2), " %*% 10^", e))
  }
  paste0("italic(p) == ", formatC(p, format = "f", digits = 4))
}

# Plotmath έκφραση για το συνολικό p του Friedman (επιστρέφει expression) με italic(p)
friedman_label_expr_fn <- function(p) {
  if (is.na(p)) {
    as.expression(bquote("Overall Friedman test:" ~ italic(p) == NA))
  } else if (p < 2.2e-16) {
    as.expression(bquote("Overall Friedman test:" ~ italic(p) < 2.2 %*% 10^-16))
  } else if (p < 1e-3) {
    e <- floor(log10(p)); m <- p / (10^e)
    as.expression(bquote("Overall Friedman test:" ~ italic(p) == .(formatC(m, format = "f", digits = 2)) %*% 10^.((e))))
  } else {
    as.expression(bquote("Overall Friedman test:" ~ italic(p) == .(formatC(p, format = "f", digits = 4))))
  }
}

# ================== 1) READ & CLEAN ==================
# Προσαρμόστε τη διαδρομή αν χρειάζεται
dat <- read_excel("C:/Users/admin/Desktop/FRBF/tbl3.xlsx")

df0 <- dat |>
  rename(DATASET = 1) |>
  pivot_longer(-DATASET, names_to = "Method", values_to = "Error") |>
  mutate(Method = toupper(trimws(Method))) |>
  filter(!is.na(Error), Error >= 0, Error <= 500)

# Επιθυμητή σειρά μεθόδων (όσες υπάρχουν θα διατηρήσουν αυτή τη σειρά)
desired_levels <- c("ADAM", "BFGS", "NEAT", "RBF", "GRBF","PROPOSED")

# Αν υπάρχουν πολλαπλές εγγραφές ανά DATASET×Method (π.χ. runs), συνοψίζουμε (mean)
df1 <- df0 |>
  group_by(DATASET, Method) |>
  summarise(Error = mean(Error, na.rm = TRUE), .groups = "drop")

present_methods <- desired_levels[desired_levels %in% unique(df1$Method)]
if (length(present_methods) < 2) stop("Χρειάζονται τουλάχιστον 2 μέθοδοι στο dataset.")

df1 <- df1 |>
  filter(Method %in% present_methods)

# ================== 2) COMMON DATASETS ΓΙΑ ΟΛΕΣ ΤΙΣ ΜΕΘΟΔΟΥΣ ==================
common_years <- df1 |>
  group_by(DATASET) |>
  summarise(nm = n_distinct(Method), .groups = "drop") |>
  filter(nm == length(present_methods)) |>
  pull(DATASET)

common_df <- df1 |>
  filter(DATASET %in% common_years)

if (nrow(common_df) < 2) stop("Δεν υπάρχουν αρκετά κοινά DATASETS για όλες τις μεθόδους.")

# ΡΗΤΑ data.frame & factors
common_df <- as.data.frame(common_df)
common_df$Method <- factor(common_df$Method, levels = present_methods)
common_df$DATASET   <- factor(common_df$DATASET)

# ================== 3) FRIEDMAN OVERALL ==================
fr <- suppressWarnings(friedman.test(Error ~ Method | DATASET, data = common_df))
friedman_p <- fr$p.value
friedman_label_expr <- friedman_label_expr_fn(friedman_p)

# ================== 4) POST-HOC (CONOVER, fallback NEMENYI) ==================
ph <- try(PMCMRplus::frdAllPairsConoverTest(Error ~ Method | DATASET, data = common_df), silent = TRUE)
if (inherits(ph, "try-error")) {
  warning("Conover post-hoc failed; falling back to Nemenyi.")
  ph <- PMCMRplus::frdAllPairsNemenyiTest(Error ~ Method | DATASET, data = common_df)
}
p_mat <- ph$p.value  # matrix p-values (ονόματα = μέθοδοι)

# Στόχος: PROPOSED vs οι υπόλοιπες, αλλιώς όλα τα ζεύγη
if ("PROPOSED" %in% present_methods) {
  target <- "PROPOSED"
  others <- setdiff(present_methods, target)
  pairs_tbl <- tibble(group1 = target, group2 = others)
} else {
  pairs_tbl <- t(combn(present_methods, 2)) |>
    as_tibble(.name_repair = "minimal") |>
    setNames(c("group1", "group2"))
}

get_pair_p <- function(a, b, P) {
  rns <- rownames(P); cns <- colnames(P)
  if (!is.null(rns) && !is.null(cns)) {
    if (a %in% rns && b %in% cns) return(P[a, b])
    if (b %in% rns && a %in% cns) return(P[b, a])
  }
  return(NA_real_)
}

pairwise_results <- pairs_tbl |>
  mutate(
    p_raw = map2_dbl(group1, group2, ~ get_pair_p(.x, .y, p_mat)),
    p_adj = p.adjust(p_raw, method = "holm"),
    p_lab = vapply(p_adj, format_p_label_expr, character(1)),  # plotmath strings
    xmin  = match(group1, present_methods),
    xmax  = match(group2, present_methods),
    y     = seq(17, by = 3, length.out = n())
  )

# ================== 5) PLOT ==================
# Κλασικοί δείκτες (geometric markers): 16=γεμάτος κύκλος, 17=τρίγωνο, 15=τετράγωνο, 18=ρόμβος, 3=+, 4=x, 8=αστέρι
shape_vals <- c(16, 17, 15, 18, 3, 4, 8)
shape_vals <- shape_vals[seq_along(present_methods)]

p <- ggplot(common_df, aes(x = Method, y = Error, color = Method)) +
  geom_boxplot(width = 0.75, outlier.shape = NA) +
  geom_jitter(aes(shape = Method), width = 0.15, size = 1.4, alpha = 0.8) +
  scale_y_continuous(labels = comma, limits = c(0, 35),
                     expand = expansion(mult = c(0.02, 0.07))) +
  scale_shape_manual(values = shape_vals) +
  labs(
    x = "", y = "Regression error",
    title = "",
    subtitle = ""
  ) +
  theme_minimal(base_size = 18) +
  theme(
    legend.position = "none",
    axis.line = element_line(color = "blue"),
    axis.ticks = element_line(color = "black"),
    axis.text = element_text(color = "blue"),
    panel.grid.major = element_line(color = "grey", linewidth = 0.5)
  ) +
  # Χειροκίνητες «γέφυρες»
  geom_segment(
    data = pairwise_results,
    aes(x = xmin, xend = xmax, y = y, yend = y),
    inherit.aes = FALSE, linewidth = 0.6
  ) +
  geom_segment(
    data = pairwise_results,
    aes(x = xmin, xend = xmin, y = y, yend = y - 0.8),
    inherit.aes = FALSE, linewidth = 0.6
  ) +
  geom_segment(
    data = pairwise_results,
    aes(x = xmax, xend = xmax, y = y, yend = y - 0.8),
    inherit.aes = FALSE, linewidth = 0.6
  ) +
  geom_text(
    data = pairwise_results,
    aes(x = (xmin + xmax)/2, y = y + 0.6, label = p_lab),
    inherit.aes = FALSE, size = 4.5, vjust = 0, parse = TRUE
  ) +
  annotate(
    "text",
    x = max(1, length(present_methods) - 3),
    y = 35,
    label = deparse(friedman_label_expr[[1]]),  # περνάει σωστά το expression ως string
    parse = TRUE,
    size = 5.5
  )

print(p)

# ================== 6) DIAGNOSTICS (console) ==================
message("Methods present (ordered): ", paste(present_methods, collapse = ", "))
message("Common DATASETS used: ", paste(sort(unique(common_df$DATASET)), collapse = ", "))
message(sprintf("Overall Friedman p = %s", formatC(friedman_p, format = "f", digits = 6)))
print(pairwise_results %>% select(group1, group2, p_raw, p_adj, p_lab))
