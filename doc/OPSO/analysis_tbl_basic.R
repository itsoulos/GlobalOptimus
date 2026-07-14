## =====================================================================
## Statistical comparison of baseline ML models vs. the proposed OPSO
## algorithm (three population/thread variants: T=1, T=2, T=5)
##
## INPUT:
##   tbl_basic.xlsx, read from INPUT_PATH below.
##   - N rows    = benchmark datasets (one row per dataset)
##   - 7 columns = models: BFGS, NEAT, RBF, GEN (baselines) and
##                 OPSO (T=1), OPSO (T=2), OPSO (T=5) (proposed method)
##   - cell values = average classification error (%) per dataset/model
##
## DESIGN NOTE:
##   Since OPSO is tested with three settings (T=1,2,5), the script
##   first determines automatically which OPSO variant has the lowest
##   overall mean error ("best OPSO variant") and uses THAT variant as
##   the reference for the pairwise significance tests against every
##   other model (the other two OPSO variants + the four baselines).
##   Change REFERENCE_MODEL manually below if you prefer to fix a
##   specific OPSO variant as the reference instead of choosing it
##   automatically.
##
## OUTPUT (written to the SAME folder as the input file):
##   1. "tbl_basic_figure.png"  - bar chart of mean error (+/- SD) per
##      model, ranked, with OPSO variants highlighted and significance
##      markers vs. the best OPSO variant.
##   2. "tbl_basic_table.xlsx"  - detailed results table: descriptive
##      stats, Friedman ranks, and paired significance tests.
## =====================================================================

## ---- 0. Path configuration ----
INPUT_PATH <- "C:/Users/admin/Desktop/OPSO/tbl_basic.xlsx"

OUTPUT_DIR  <- dirname(INPUT_PATH)
FIGURE_PATH <- file.path(OUTPUT_DIR, "tbl_basic_figure.png")
TABLE_PATH  <- file.path(OUTPUT_DIR, "tbl_basic_table.xlsx")

## Set to NULL to auto-select the best-performing OPSO variant as the
## reference model, or set to one of "OPSO_T1","OPSO_T2","OPSO_T5" to
## fix it manually.
REFERENCE_MODEL <- NULL

## ---- 1. Setup ----
required_packages <- c("readxl", "dplyr", "tidyr", "ggplot2", "openxlsx")
to_install <- required_packages[!required_packages %in% installed.packages()[, "Package"]]
if (length(to_install) > 0) install.packages(to_install, repos = "https://cloud.r-project.org")

library(readxl)
library(dplyr)
library(tidyr)
library(ggplot2)
library(openxlsx)

## ---- 2. Load data ----
raw <- read_excel(INPUT_PATH, sheet = 1)
names(raw) <- trimws(names(raw))                 # strip stray trailing spaces
names(raw)[names(raw) == "DATASET"] <- "Dataset"

## Clean model names (drop parentheses/spaces -> valid R names)
model_name_map <- c(
  "BFGS" = "BFGS", "NEAT" = "NEAT", "RBF" = "RBF", "GEN" = "GEN",
  "OPSO (T=1)" = "OPSO_T1", "OPSO (T=2)" = "OPSO_T2", "OPSO (T=5)" = "OPSO_T5"
)
raw_cols_trimmed <- trimws(names(raw)[-1])
names(raw)[-1] <- model_name_map[raw_cols_trimmed]

dataset_names <- raw$Dataset
model_cols    <- setdiff(names(raw), "Dataset")
opso_group    <- intersect(c("OPSO_T1", "OPSO_T2", "OPSO_T5"), model_cols)
baseline_group <- setdiff(model_cols, opso_group)

data_wide <- raw
data_long <- data_wide %>%
  pivot_longer(cols = -Dataset, names_to = "Model", values_to = "Error") %>%
  mutate(
    Dataset = factor(Dataset, levels = dataset_names),
    Model   = factor(Model, levels = model_cols)
  )

error_matrix <- as.matrix(data_wide[, model_cols])
rownames(error_matrix) <- dataset_names

## ---- 3. Descriptive statistics per model ----
desc_stats <- data_long %>%
  group_by(Model) %>%
  summarise(
    Mean_Error = mean(Error),
    SD_Error   = sd(Error),
    Min_Error  = min(Error),
    Max_Error  = max(Error),
    .groups = "drop"
  )

## ---- 4. Friedman test (overall comparison across all 7 models) ----
friedman_result <- friedman.test(error_matrix)

## ---- 5. Average ranks + Nemenyi critical difference ----
k <- ncol(error_matrix)
n <- nrow(error_matrix)

rank_matrix <- t(apply(error_matrix, 1, rank))   # 1 = best (lowest error) within each dataset
avg_ranks   <- colMeans(rank_matrix)

q_alpha_table <- c("2"=1.960, "3"=2.343, "4"=2.569, "5"=2.728,
                   "6"=2.850, "7"=2.949, "8"=3.031, "9"=3.102, "10"=3.164)
q_alpha <- as.numeric(q_alpha_table[as.character(k)])
CD <- q_alpha * sqrt(k * (k + 1) / (6 * n))

## ---- 6. Determine the reference model (best OPSO variant) ----
if (is.null(REFERENCE_MODEL)) {
  opso_means <- desc_stats %>% filter(Model %in% opso_group)
  REFERENCE_MODEL <- as.character(opso_means$Model[which.min(opso_means$Mean_Error)])
}
cat("Reference model (best OPSO variant) selected as:", REFERENCE_MODEL, "\n")

## ---- 7. Paired comparisons: reference OPSO variant vs. every other model ----
reference_errors <- data_wide[[REFERENCE_MODEL]]
other_models <- setdiff(model_cols, REFERENCE_MODEL)

pairwise_tests <- lapply(other_models, function(mdl) {
  other_errors <- data_wide[[mdl]]
  wilcox_p <- tryCatch(
    wilcox.test(reference_errors, other_errors, paired = TRUE, exact = FALSE)$p.value,
    error = function(e) NA)
  ttest_p <- tryCatch(
    t.test(reference_errors, other_errors, paired = TRUE)$p.value,
    error = function(e) NA)
  data.frame(
    Model = mdl,
    Mean_Diff_vs_Reference = mean(reference_errors) - mean(other_errors),
    Wilcoxon_p = wilcox_p,
    Paired_t_p = ttest_p
  )
})
pairwise_tests <- do.call(rbind, pairwise_tests)

## ---- 8. Assemble the full results table ----
results_table <- desc_stats %>%
  mutate(Model = as.character(Model)) %>%
  left_join(data.frame(Model = names(avg_ranks), Avg_Rank = avg_ranks), by = "Model") %>%
  left_join(pairwise_tests %>% mutate(Model = as.character(Model)), by = "Model") %>%
  mutate(
    Group = ifelse(Model %in% opso_group, "Proposed (OPSO)", "Baseline"),
    Mean_Diff_vs_Reference = ifelse(Model == REFERENCE_MODEL, 0, Mean_Diff_vs_Reference),
    Significance = case_when(
      Model == REFERENCE_MODEL ~ "Reference",
      is.na(Paired_t_p)        ~ NA_character_,
      Paired_t_p < 0.001       ~ "***",
      Paired_t_p < 0.01        ~ "**",
      Paired_t_p < 0.05        ~ "*",
      TRUE                     ~ "n.s."
    )
  ) %>%
  arrange(Mean_Error) %>%
  mutate(
    Mean_Error = round(Mean_Error, 2),
    SD_Error   = round(SD_Error, 2),
    Min_Error  = round(Min_Error, 2),
    Max_Error  = round(Max_Error, 2),
    Avg_Rank   = round(Avg_Rank, 2),
    Mean_Diff_vs_Reference = round(Mean_Diff_vs_Reference, 2),
    Wilcoxon_p = round(Wilcoxon_p, 4),
    Paired_t_p = round(Paired_t_p, 4)
  ) %>%
  select(Model, Group, Mean_Error, SD_Error, Min_Error, Max_Error, Avg_Rank,
         Mean_Diff_vs_Reference, Wilcoxon_p, Paired_t_p, Significance)

cat("\n================ RESULTS TABLE ================\n")
print(results_table, row.names = FALSE)

cat("\nFriedman test (overall comparison across all", k, "models):\n")
cat("  chi-squared =", round(friedman_result$statistic, 3),
    ", df =", friedman_result$parameter,
    ", p-value =", round(friedman_result$p.value, 4), "\n")
cat("Nemenyi critical difference (alpha = 0.05):", round(CD, 3), "\n")
cat("\nSignificance codes: *** p<0.001  ** p<0.01  * p<0.05  n.s. = not significant\n")
cat("(based on paired t-test vs.", REFERENCE_MODEL, "across the", n, "datasets)\n")

## ---- 9. Save the results table as a formatted .xlsx workbook ----
wb <- createWorkbook()
addWorksheet(wb, "Statistical_Analysis")

writeData(wb, "Statistical_Analysis", results_table, startRow = 1, headerStyle = createStyle(
  textDecoration = "bold", fgFill = "#D9D9D9", border = "TopBottom"
))

notes_row <- nrow(results_table) + 3
writeData(wb, "Statistical_Analysis",
          x = data.frame(Notes = c(
            paste0("Friedman test: chi-squared = ", round(friedman_result$statistic, 3),
                   ", df = ", friedman_result$parameter,
                   ", p-value = ", round(friedman_result$p.value, 4)),
            paste0("Nemenyi critical difference (alpha = 0.05): ", round(CD, 3)),
            paste0("Reference model for pairwise tests: ", REFERENCE_MODEL,
                   " (auto-selected as the best-performing OPSO variant)"),
            "Significance vs. reference (paired t-test): *** p<0.001, ** p<0.01, * p<0.05, n.s. = not significant",
            paste0("Note: n = ", n, " benchmark datasets per model.")
          )),
          startRow = notes_row, colNames = FALSE)

setColWidths(wb, "Statistical_Analysis", cols = 1:11, widths = "auto")
saveWorkbook(wb, TABLE_PATH, overwrite = TRUE)
cat("\nSaved table to:", TABLE_PATH, "\n")

## ---- 10. Build the figure ----
plot_data <- desc_stats %>%
  mutate(Model = as.character(Model)) %>%
  left_join(results_table %>% select(Model, Significance), by = "Model") %>%
  mutate(
    Model = factor(Model, levels = Model[order(Mean_Error)]),
    Label = case_when(
      Significance == "Reference" ~ "best",
      Significance == "***"       ~ "***",
      Significance == "**"        ~ "**",
      Significance == "*"         ~ "*",
      TRUE                        ~ "n.s."
    ),
    Highlight = ifelse(Model %in% opso_group, "Proposed (OPSO)", "Baseline")
  )

fig <- ggplot(plot_data, aes(x = Model, y = Mean_Error, fill = Highlight)) +
  geom_col(width = 0.65, colour = "black", linewidth = 0.3) +
  geom_errorbar(aes(ymin = Mean_Error - SD_Error, ymax = Mean_Error + SD_Error),
                width = 0.2, linewidth = 0.5) +
  geom_text(aes(label = Label, y = Mean_Error + SD_Error + 1.2),
            size = 4.2, fontface = "bold") +
  geom_text(aes(label = paste0(round(Mean_Error, 2), "%"), y = Mean_Error + SD_Error + 3.5),
            size = 3.6) +
  scale_fill_manual(values = c("Proposed (OPSO)" = "#2E7D32", "Baseline" = "#90A4AE")) +
  labs(
    title = "",
    subtitle = paste0("Friedman test: chi-squared = ", round(friedman_result$statistic, 2),
                       ", p = ", round(friedman_result$p.value, 3),
                       "  |  Significance markers show paired t-test vs. ", REFERENCE_MODEL),
    x = NULL,
    y = "Mean classification error [%] \u00b1 SD",
    fill = NULL
  ) +
  theme_minimal(base_size = 12) +
  theme(
    plot.subtitle = element_text(size = 9.5, colour = "grey30"),
    legend.position = "top",
    panel.grid.minor = element_blank(),
    axis.text.x = element_text(size = 10, angle = 20, hjust = 1)
  ) +
  ylim(0, max(plot_data$Mean_Error + plot_data$SD_Error) + 6)

ggsave(FIGURE_PATH, fig, width = 9, height = 5.5, dpi = 300)
cat("Saved figure to:", FIGURE_PATH, "\n")

cat("\n================ END OF ANALYSIS ================\n")
