## =====================================================================
## Statistical analysis of the effect of population size (Nc) on the
## performance of the proposed OPSO algorithm
##
## INPUT:
##   tbl_part.xlsx, read from INPUT_PATH below.
##   - N rows = benchmark datasets (one row per dataset)
##   - 4 columns = OPSO with population sizes Nc=200, Nc=500,
##                 Nc=1000, Nc=2000
##   - cell values = average classification error (%) per dataset/Nc
##
## DESIGN NOTE:
##   To keep this in line with the other two analyses, the script
##   automatically determines which population size has the lowest
##   overall mean error ("best population size") and uses THAT size as
##   the reference for the pairwise significance tests against every
##   other population size - same pattern as the OPSO-variant reference
##   in the tbl_basic script and the NtoN reference in the tbl_prop
##   script. Change REFERENCE_MODEL manually below to fix a specific
##   population size as the reference instead of choosing it
##   automatically. The full pairwise comparison between ALL population
##   sizes (all 6 pairs, Holm-corrected) is still provided in a second
##   sheet of the output table for completeness.
##
## OUTPUT (written to the SAME folder as the input file):
##   1. "tbl_part_figure.png" - bar chart of mean error (+/- SD) per
##      population size, ranked, with the best size highlighted and
##      significance markers vs. that reference.
##   2. "tbl_part_table.xlsx" - descriptive stats + Friedman ranks +
##      paired significance tests vs. the reference (sheet 1) + full
##      pairwise significance matrix, Holm-corrected (sheet 2).
## =====================================================================

## ---- 0. Path configuration ----
INPUT_PATH <- "C:/Users/admin/Desktop/OPSO/tbl_part.xlsx"

OUTPUT_DIR  <- dirname(INPUT_PATH)
FIGURE_PATH <- file.path(OUTPUT_DIR, "tbl_part_figure.png")
TABLE_PATH  <- file.path(OUTPUT_DIR, "tbl_part_table.xlsx")

## Set to NULL to auto-select the best-performing population size as
## the reference model, or set to one of "Nc200","Nc500","Nc1000","Nc2000"
## to fix it manually.
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
names(raw) <- trimws(names(raw))
names(raw)[names(raw) == "DATASET"] <- "Dataset"

## Clean names: "Nc=200" -> "Nc200" etc.
size_map <- setNames(gsub("[^A-Za-z0-9]", "", names(raw)[-1]), names(raw)[-1])
names(raw)[-1] <- size_map

dataset_names <- raw$Dataset
model_cols    <- names(raw)[-1]
## Natural population-size order (used for the pairwise-matrix sheet only)
pop_sizes        <- as.numeric(gsub("[^0-9]", "", model_cols))
model_cols_sized <- model_cols[order(pop_sizes)]

data_wide <- raw
data_long <- data_wide %>%
  pivot_longer(cols = -Dataset, names_to = "Model", values_to = "Error") %>%
  mutate(
    Dataset = factor(Dataset, levels = dataset_names),
    Model   = factor(Model, levels = model_cols)
  )

error_matrix <- as.matrix(data_wide[, model_cols])
rownames(error_matrix) <- dataset_names

## ---- 3. Descriptive statistics per population size ----
desc_stats <- data_long %>%
  group_by(Model) %>%
  summarise(
    Mean_Error = mean(Error),
    SD_Error   = sd(Error),
    Min_Error  = min(Error),
    Max_Error  = max(Error),
    .groups = "drop"
  )

## ---- 4. Friedman test (overall comparison across all population sizes) ----
friedman_result <- friedman.test(error_matrix)

## ---- 5. Average ranks + Nemenyi critical difference ----
k <- ncol(error_matrix)
n <- nrow(error_matrix)

rank_matrix <- t(apply(error_matrix, 1, rank))
avg_ranks   <- colMeans(rank_matrix)

q_alpha_table <- c("2"=1.960, "3"=2.343, "4"=2.569, "5"=2.728,
                   "6"=2.850, "7"=2.949, "8"=3.031, "9"=3.102, "10"=3.164)
q_alpha <- as.numeric(q_alpha_table[as.character(k)])
CD <- q_alpha * sqrt(k * (k + 1) / (6 * n))

## ---- 6. Determine the reference model (best population size) ----
if (is.null(REFERENCE_MODEL)) {
  REFERENCE_MODEL <- as.character(desc_stats$Model[which.min(desc_stats$Mean_Error)])
}
cat("Reference model (best population size) selected as:", REFERENCE_MODEL, "\n")

## ---- 7. Paired comparisons: reference population size vs. every other ----
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

## ---- 8. Assemble the main results table ----
results_table <- desc_stats %>%
  mutate(Model = as.character(Model)) %>%
  left_join(data.frame(Model = names(avg_ranks), Avg_Rank = avg_ranks), by = "Model") %>%
  left_join(pairwise_tests %>% mutate(Model = as.character(Model)), by = "Model") %>%
  mutate(
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
  select(Model, Mean_Error, SD_Error, Min_Error, Max_Error, Avg_Rank,
         Mean_Diff_vs_Reference, Wilcoxon_p, Paired_t_p, Significance)

cat("\n================ RESULTS TABLE ================\n")
print(results_table, row.names = FALSE)

## ---- 9. Full pairwise comparisons between ALL population sizes (extra sheet) ----
pairs <- combn(model_cols_sized, 2, simplify = FALSE)

full_pairwise <- lapply(pairs, function(p) {
  e1 <- data_wide[[p[1]]]
  e2 <- data_wide[[p[2]]]
  wilcox_p <- tryCatch(wilcox.test(e1, e2, paired = TRUE, exact = FALSE)$p.value, error = function(e) NA)
  ttest_p  <- tryCatch(t.test(e1, e2, paired = TRUE)$p.value, error = function(e) NA)
  data.frame(
    Comparison = paste(p[1], "vs", p[2]),
    Mean_Diff  = mean(e1) - mean(e2),
    Wilcoxon_p = wilcox_p,
    Paired_t_p = ttest_p
  )
})
full_pairwise <- do.call(rbind, full_pairwise)
full_pairwise$Wilcoxon_p_holm <- p.adjust(full_pairwise$Wilcoxon_p, method = "holm")
full_pairwise$Paired_t_p_holm <- p.adjust(full_pairwise$Paired_t_p, method = "holm")
full_pairwise <- full_pairwise %>%
  mutate(
    Significance = case_when(
      is.na(Paired_t_p_holm) ~ NA_character_,
      Paired_t_p_holm < 0.001 ~ "***",
      Paired_t_p_holm < 0.01  ~ "**",
      Paired_t_p_holm < 0.05  ~ "*",
      TRUE                    ~ "n.s."
    ),
    Mean_Diff       = round(Mean_Diff, 2),
    Wilcoxon_p      = round(Wilcoxon_p, 4),
    Paired_t_p      = round(Paired_t_p, 4),
    Wilcoxon_p_holm = round(Wilcoxon_p_holm, 4),
    Paired_t_p_holm = round(Paired_t_p_holm, 4)
  )

cat("\nFriedman test (overall comparison across all", k, "population sizes):\n")
cat("  chi-squared =", round(friedman_result$statistic, 3),
    ", df =", friedman_result$parameter,
    ", p-value =", round(friedman_result$p.value, 4), "\n")
cat("Nemenyi critical difference (alpha = 0.05):", round(CD, 3), "\n")
cat("\nSignificance codes: *** p<0.001  ** p<0.01  * p<0.05  n.s. = not significant\n")
cat("(based on paired t-test vs.", REFERENCE_MODEL, "across the", n, "datasets)\n")

## ---- 10. Save results to a formatted .xlsx workbook (2 sheets) ----
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
                   " (auto-selected as the best-performing population size)"),
            "Significance vs. reference (paired t-test): *** p<0.001, ** p<0.01, * p<0.05, n.s. = not significant",
            paste0("Note: n = ", n, " benchmark datasets per population size."),
            "See 'Full_Pairwise' sheet for all 6 pairwise comparisons (Holm-corrected)."
          )),
          startRow = notes_row, colNames = FALSE)
setColWidths(wb, "Statistical_Analysis", cols = 1:10, widths = "auto")

addWorksheet(wb, "Full_Pairwise")
writeData(wb, "Full_Pairwise", full_pairwise, startRow = 1, headerStyle = createStyle(
  textDecoration = "bold", fgFill = "#D9D9D9", border = "TopBottom"
))
setColWidths(wb, "Full_Pairwise", cols = 1:7, widths = "auto")

saveWorkbook(wb, TABLE_PATH, overwrite = TRUE)
cat("\nSaved table to:", TABLE_PATH, "\n")

## ---- 11. Build the figure ----
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
    Highlight = ifelse(Model == REFERENCE_MODEL, "Best population size", "Other population sizes")
  )

fig <- ggplot(plot_data, aes(x = Model, y = Mean_Error, fill = Highlight)) +
  geom_col(width = 0.6, colour = "black", linewidth = 0.3) +
  geom_errorbar(aes(ymin = Mean_Error - SD_Error, ymax = Mean_Error + SD_Error),
                width = 0.2, linewidth = 0.5) +
  geom_text(aes(label = Label, y = Mean_Error + SD_Error + 1.2),
            size = 4.2, fontface = "bold") +
  geom_text(aes(label = paste0(round(Mean_Error, 2), "%"), y = Mean_Error + SD_Error + 3.5),
            size = 3.6) +
  scale_fill_manual(values = c("Best population size" = "#2E7D32", "Other population sizes" = "#90A4AE")) +
  labs(
    title = "",
    subtitle = paste0("Friedman test: chi-squared = ", round(friedman_result$statistic, 2),
                       ", p = ", round(friedman_result$p.value, 3),
                       "  |  Significance markers show paired t-test vs. ", REFERENCE_MODEL),
    x = "OPSO population size",
    y = "Mean classification error [%] \u00b1 SD",
    fill = NULL
  ) +
  theme_minimal(base_size = 12) +
  theme(
    plot.subtitle = element_text(size = 9.5, colour = "grey30"),
    legend.position = "top",
    panel.grid.minor = element_blank(),
    axis.text.x = element_text(size = 10)
  ) +
  ylim(0, max(plot_data$Mean_Error + plot_data$SD_Error) + 6)

ggsave(FIGURE_PATH, fig, width = 8, height = 5.5, dpi = 300)
cat("Saved figure to:", FIGURE_PATH, "\n")

cat("\n================ END OF ANALYSIS ================\n")
