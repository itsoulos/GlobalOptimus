library(tidyverse)
library(readxl)
library(ggpubr)
library(scales)
library(ggplot2)
library(ggsignif)

# Διαβάζουμε τα δεδομένα
dat <- read_excel("C:/Users/admin/Desktop/NNC_PRETRAIN/t2.xlsx")
df <- dat |>
  rename(DATASET = 1) |>
  pivot_longer(-DATASET, names_to = "Method", values_to = "Error") |>
  filter(!is.na(Error) & Error >= 0 & Error <= 310)  # Φιλτράρουμε τα δεδομένα

# Ορίζουμε τις συγκρίσεις
cmp <- list(
  c("PROPOSED", "ADAM"),
  c("PROPOSED", "BFGS"),
  c("PROPOSED", "GENETIC"),
  c("PROPOSED", "RBF"),
  c("PROPOSED", "NEAT"),
  c("PROPOSED", "PRUNE"),
  c("PROPOSED", "NNC")
)

# Δημιουργούμε το boxplot με τις στατιστικές συγκρίσεις
ggboxplot(df, x = "Method", y = "Error", color = "Method", shape = "Method", add = "jitter",
          palette = NULL, width = 0.8, add.params = list(size = 1.5)) +

stat_compare_means(
  paired = TRUE,
  method = "wilcox.test",
  comparisons = cmp,
  label = "p.signif",
  label.y = seq(150, 250, by = 15),
  tip_length = 0.001,
  size = 4
) +  # Προσαρμογή ετικετών

  #annotate("text", x = 2.7, y = 85, label = "Overall Wilcoxon Test: p < 0.05", size = 4, color = "black", hjust = 0.5) +

  labs(x = "", y = "Regression error",
       title = "Method Comparison",
       subtitle = "Pairwise Wilcoxon tests") +

  theme_minimal(base_size = 15) +

  scale_y_continuous(labels = comma, limits = c(0, 310)) +

  # Χειροκίνητα ορισμός 8 χρωμάτων
  scale_color_manual(values = c(
    "PROPOSED" = "#E41A1C",
    "ADAM"     = "#377EB8",
    "BFGS"     = "#4DAF4A",
    "GENETIC"  = "#984EA3",
    "RBF"      = "#FF7F00",
    "NEAT"     = "#FFFF33",
    "PRUNE"    = "#A65628",
    "NNC"      = "#F781BF"
  )) +

  # Ορισμός 8 σχημάτων για τις μεθόδους
  scale_shape_manual(values = c(15, 16, 17, 18, 19, 20, 21, 22)) +

  theme(
    legend.position = "none",
    axis.line = element_line(color = "blue", linewidth = 0.5),
    axis.ticks = element_line(color = "black"),
    axis.text = element_text(color = "blue"),
    panel.grid.major = element_line(color = "grey", linewidth = 0.5)
  ) +

  annotate("text", x = 3.0, y = 275,
           label = paste0("*: p < 0.05 (Very significant\n**: p < 0.01 (Highly significant)\n***: p < 0.001  (Εxtremely significant)\n****: p < 0.0001  (Very extremely significant)"),
           hjust = 0, size = 2, color = "blue")
