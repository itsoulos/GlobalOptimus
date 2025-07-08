library(tidyverse)
library(readxl)
library(ggpubr)
library(scales)
library(ggplot2)

# Διαβάζουμε τα δεδομένα
dat <- read_excel("C:/Users/admin/Desktop/NNC_PRETRAIN/t3.xlsx")
df <- dat |>
  rename(DATASET = 1) |>
  pivot_longer(-DATASET, names_to = "Method", values_to = "Error") |>
  filter(!is.na(Error) & Error >= 0 & Error <= 100)  # Φιλτράρουμε τα δεδομένα

# Ορίζουμε τις συγκρίσεις
cmp <- list(
  c("Iw=2", "Iw=3"),
  c("Iw=3", "Iw=5"),
  c("Iw=5", "Iw=10")
)


# Δημιουργούμε το boxplot με τις στατιστικές συγκρίσεις
ggboxplot(df, x = "Method", y = "Error", color = "Method", shape = "Method", add = "jitter",
          palette = "jco", width = 0.8, add.params = list(size = 1.5)) +
  # Μόνο μία κλήση για όλες τις συγκρίσεις
  stat_compare_means(paired = TRUE, method = "wilcox.test", comparisons = cmp, label = "p.signif",
                     label.y = c(56, 60, 64), size = 4) +  # Ειδικές συγκρίσεις
  # Προσθέτουμε το γενικό test ως annotation, όχι μέσα στο plot
  #annotate("text", x = 2.7, y = 80, label = "Overall Wilcoxon Test: p < 0.05", size = 4, color = "black", hjust = 0.5) +
  labs(x = "", y = "Classification error (%)",
       title = "Method Comparison",
       subtitle = "Pairwise Wilcoxon tests") +
  theme_minimal(base_size = 15) +
  scale_y_continuous(labels = comma, limits = c(0, 90)) +  # Αυξάνουμε το όριο του άξονα Υ
  scale_shape_manual(values = c(15, 16, 17, 18, 19, 20, 21)) +  # Χειροκίνητα σχήματα
  theme(
    legend.position = "none",
    axis.line = element_line(color = "blue"),
    axis.ticks = element_line(color = "black"),
    axis.text = element_text(color = "blue"),
    panel.grid.major = element_line(color = "grey", size = 0.5)
  ) +
   #Προσθέτουμε επεξήγηση για τα επίπεδα σημαντικότητας
  annotate("text", x = 2.8, y = 80,
           label = paste0(

             "ns: p > 0.05 (Not significant)"
           ),
           hjust = 0, size = 3, color = "blue")  # Μικρότερο μέγεθος κειμένου
