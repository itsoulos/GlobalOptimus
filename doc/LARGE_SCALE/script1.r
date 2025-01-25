library(tidyverse)
library(readxl)
library(ggpubr)
library(scales)
library(ggplot2)

# Διαβάζουμε τα δεδομένα
dat <- read_excel("C:/Users/admin/Desktop/LARGE_SCALE/t1.xlsx")
df <- dat |>
  rename(FUNCTION = 1) |>
  pivot_longer(-c(FUNCTION, DIM), names_to = "Method", values_to = "Error")

# Ορίζουμε τις συγκρίσεις
cmp <- list(
  c("NUMBER(R)", "RANDOM(R)"),
  c("NUMBER(R)", "MIGRANT(R)"),
  c("RANDOM(R)", "MIGRANT(R)")
)

# Δημιουργούμε το boxplot με τις στατιστικές συγκρίσεις
ggboxplot(df, x = "Method", y = "Error", color = "Method", shape = "Method", add = "jitter",
          palette = "jco", width = 0.8, add.params = list(size = 1.5)) +
  # Προσθέτουμε τις συγκρίσεις με προσαρμοσμένα label.y για αποφυγή επικαλύψεων
  stat_compare_means(paired = TRUE, method = "t.test", comparisons = cmp, label = "p.signif",
                     label.y = c(40000, 45000, 50000)) +
  stat_compare_means(paired = TRUE, var.equal = FALSE, label.y = 67000, label.x = 1) +
  labs(x = "", y = "FUNCTION CALLS",
       title = "",
       subtitle = "Pairwise t-tests") +
  theme_minimal(base_size = 15) +
  scale_y_continuous(labels = comma, limits = c(0, 70000)) +
  theme(
    legend.position = "none",
    axis.line = element_line(color = "blue"),     # Προσθέτουμε γραμμές στους άξονες
    axis.ticks = element_line(color = "black"),    # Προσθέτουμε τα ticks στους άξονες
    axis.text = element_text(color = "blue"),     # Χρωματίζουμε τα κείμενα των αξόνων
    panel.grid.major = element_line(color = "grey", size = 0.5)  # Ενεργοποιούμε το κύριο πλέγμα
   # panel.grid.minor = element_line(color = "lightgrey", size = 0.25) # Ενεργοποιούμε το δευτερεύον πλέγμα
  ) +
  # Προσθέτουμε επεξήγηση για τα επίπεδα σημαντικότητας
  annotate("text", x = 2.5, y = 70000,
           label = paste0(
             "*: p < 0.05 (Significant)\n",
             "**: p < 0.01 (Highly significant)\n",
             "***: p < 0.001 (Extremely significant)\n",
             "****: p < 0.0001 (Very extremely significant)"
           ),
           hjust = 0, size = 2, color = "blue")  # Μικρότερο μέγεθος κειμένου
