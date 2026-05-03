# ============================================================
#  Wilcoxon Signed-Rank Test — ARQ2 vs all competitors
#  Pairwise comparisons: best-value rankings (Table 6)
#                    and mean-value rankings  (Table 7)
#  Output: console results + PNG figure
# ============================================================

library(readxl)
library(ggplot2)
library(tidyr)
library(dplyr)

# --- File paths ---
path_summary  <- "C:/Users/admin/Desktop/arq2/xlsx/tbl.xlsx"
path_best     <- "C:/Users/admin/Desktop/arq2/xlsx/bestranking.xlsx"
path_mean     <- "C:/Users/admin/Desktop/arq2/xlsx/meanRanking.xlsx"
path_out_csv  <- "C:/Users/admin/Desktop/arq2/xlsx/wilcoxon_results.csv"
path_out_png  <- "C:/Users/admin/Desktop/arq2/xlsx/wilcoxon_plot.png"

# --- Load data ---
dat       <- read_excel(path_summary)
rank_best <- read_excel(path_best)
rank_mean <- read_excel(path_mean)

cat("Summary table (Table 8):\n")
print(dat)
cat("\n")

# ============================================================
#  Wilcoxon Signed-Rank Test function
# ============================================================

run_wilcoxon <- function(rank_data, label) {

  competitors <- setdiff(names(rank_data), c("PROBLEM", "DIM", "arq2"))

  results <- data.frame(
    Competitor   = character(),
    W_statistic  = numeric(),
    p_value      = numeric(),
    p_adjusted   = numeric(),
    Significance = character(),
    stringsAsFactors = FALSE
  )

  arq2_ranks <- as.numeric(rank_data$arq2)

  for (comp in competitors) {
    comp_ranks <- as.numeric(rank_data[[comp]])
    diffs      <- arq2_ranks - comp_ranks
    non_zero   <- diffs[diffs != 0]

    if (length(non_zero) < 5) {
      results <- rbind(results, data.frame(
        Competitor   = comp,
        W_statistic  = NA,
        p_value      = NA,
        p_adjusted   = NA,
        Significance = "insufficient pairs"
      ))
      next
    }

    wt <- wilcox.test(arq2_ranks, comp_ranks,
                      paired      = TRUE,
                      alternative = "two.sided",
                      exact       = FALSE)

    results <- rbind(results, data.frame(
      Competitor   = comp,
      W_statistic  = as.numeric(wt$statistic),
      p_value      = round(wt$p.value, 4),
      p_adjusted   = NA,
      Significance = ""
    ))
  }

  valid <- !is.na(results$p_value)
  results$p_adjusted[valid] <- round(
    p.adjust(results$p_value[valid], method = "bonferroni"), 4)

  results$Significance <- ifelse(
    is.na(results$p_adjusted), "—",
    ifelse(results$p_adjusted < 0.001, "***",
    ifelse(results$p_adjusted < 0.01,  "**",
    ifelse(results$p_adjusted < 0.05,  "*",  "ns"))))

  cat("\n", strrep("=", 65), "\n", sep = "")
  cat(" Wilcoxon Signed-Rank: ARQ2 vs competitors\n")
  cat(" [", label, "]\n", sep = "")
  cat(" Significance: * p<0.05  ** p<0.01  *** p<0.001  ns = not significant\n")
  cat(strrep("=", 65), "\n", sep = "")
  print(results, row.names = FALSE)

  return(invisible(results))
}

# ============================================================
#  Run tests
# ============================================================

res_best <- run_wilcoxon(rank_best, "Best-value rankings — Table 6")
res_mean <- run_wilcoxon(rank_mean, "Mean-value rankings — Table 7")

# ============================================================
#  Combined summary table
# ============================================================

combined <- merge(
  res_best[, c("Competitor", "p_value", "p_adjusted", "Significance")],
  res_mean[, c("Competitor", "p_value", "p_adjusted", "Significance")],
  by       = "Competitor",
  suffixes = c("_best", "_mean")
)

combined <- combined[order(combined$p_adjusted_mean, na.last = TRUE), ]

cat("\n", strrep("=", 70), "\n", sep = "")
cat(" Combined summary: ARQ2 vs all competitors (Best + Mean rankings)\n")
cat(strrep("=", 70), "\n", sep = "")
print(combined, row.names = FALSE)

write.csv(combined, path_out_csv, row.names = FALSE)
cat("\nResults saved to:", path_out_csv, "\n")

# ============================================================
#  Figure: dot plot with p-values and significance labels
# ============================================================

# Prepare long-format data for plotting
plot_data <- bind_rows(
  data.frame(
    Competitor   = res_best$Competitor,
    p_adjusted   = res_best$p_adjusted,
    Significance = res_best$Significance,
    Criterion    = "Best-value (Table 6)",
    stringsAsFactors = FALSE
  ),
  data.frame(
    Competitor   = res_mean$Competitor,
    p_adjusted   = res_mean$p_adjusted,
    Significance = res_mean$Significance,
    Criterion    = "Mean-value (Table 7)",
    stringsAsFactors = FALSE
  )
)

# Remove rows with no valid p-value
plot_data <- plot_data[!is.na(plot_data$p_adjusted), ]

# Cap very small p-values at 1e-4 for display
plot_data$p_plot <- pmax(plot_data$p_adjusted, 1e-4)

# Ordered competitor axis (by mean p_adjusted, ascending)
order_comps <- combined$Competitor[order(combined$p_adjusted_mean)]
order_comps <- order_comps[order_comps %in% plot_data$Competitor]
plot_data$Competitor <- factor(plot_data$Competitor, levels = rev(order_comps))

# Significance colour palette
sig_colors <- c(
  "***" = "#1a6e2e",
  "**"  = "#3aab57",
  "*"   = "#85c98a",
  "ns"  = "#b0b0b0",
  "—"   = "#d0d0d0"
)

# Significance label positions (slightly above each point)
label_y <- plot_data$p_plot * 1.6

p <- ggplot(plot_data,
            aes(x     = Competitor,
                y     = p_plot,
                color = Significance,
                shape = Criterion)) +

  # Reference lines
  geom_hline(yintercept = 0.05,  linetype = "dashed",
             color = "#e07b39", linewidth = 0.6) +
  geom_hline(yintercept = 0.01,  linetype = "dashed",
             color = "#c0392b", linewidth = 0.6) +
  geom_hline(yintercept = 0.001, linetype = "dashed",
             color = "#7d1a1a", linewidth = 0.6) +

  # Connecting lines between Best and Mean for same competitor
  geom_line(aes(group = Competitor),
            color = "grey70", linewidth = 0.5, alpha = 0.6) +

  # Points
  geom_point(size = 4, stroke = 1.1) +

  # Significance labels above points
  geom_text(aes(label = Significance, y = label_y),
            size = 3.8, fontface = "bold",
            show.legend = FALSE) +

  # Threshold annotations on right margin
  annotate("text", x = Inf, y = 0.05,  label = "p = 0.05",
           hjust = 1.05, vjust = -0.4, size = 3, color = "#e07b39") +
  annotate("text", x = Inf, y = 0.01,  label = "p = 0.01",
           hjust = 1.05, vjust = -0.4, size = 3, color = "#c0392b") +
  annotate("text", x = Inf, y = 0.001, label = "p = 0.001",
           hjust = 1.05, vjust = -0.4, size = 3, color = "#7d1a1a") +

  scale_y_log10(
    limits = c(8e-5, 1.5),
    breaks = c(0.001, 0.01, 0.05, 0.1, 0.5, 1.0),
    labels = c("0.001", "0.01", "0.05", "0.1", "0.5", "1.0")
  ) +

  scale_color_manual(
    values = sig_colors,
    name   = "Significance\n(Bonferroni)",
    breaks = c("***", "**", "*", "ns")
  ) +

  scale_shape_manual(
    values = c("Best-value (Table 6)" = 16,
               "Mean-value (Table 7)" = 17),
    name   = "Criterion"
  ) +

  coord_flip() +

  labs(
    title    = "Wilcoxon Signed-Rank Test: ARQ2 vs Competitors",
    subtitle = "Pairwise comparisons with Bonferroni correction",
    x        = NULL,
    y        = "Adjusted p-value (log scale)"
  ) +

  theme_bw(base_size = 13) +
  theme(
    plot.title       = element_text(face = "bold", size = 14),
    plot.subtitle    = element_text(color = "grey40", size = 11),
    legend.position  = "right",
    legend.title     = element_text(size = 10, face = "bold"),
    legend.text      = element_text(size = 9),
    panel.grid.minor = element_blank(),
    axis.text.y      = element_text(size = 11, face = "bold"),
    axis.text.x      = element_text(size = 10)
  )

ggsave(path_out_png,
       plot   = p,
       width  = 10,
       height = 6.5,
       dpi    = 300,
       bg     = "white")

cat("\nFigure saved to:", path_out_png, "\n")
