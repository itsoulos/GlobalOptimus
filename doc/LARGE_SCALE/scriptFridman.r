# Φόρτωση βιβλιοθηκών
library(tidyverse)
library(rstatix)
library(ggpubr)

# Εισαγωγή δεδομένων
dat <- read_excel("C:/Users/admin/Desktop/LARGE_SCALE/t5.xlsx")

# Αναδιάρθρωση δεδομένων
df <- dat %>%
  pivot_longer(cols = c(AQUILA, AOA, SAO, PROPOSED),
               names_to = "Method",
               values_to = "Error") %>%
  mutate(FUNCTION = as.factor(FUNCTION))

# Υπολογισμός Friedman Test
friedman_result <- friedman_test(Error ~ Method | FUNCTION, data = df)

# Εμφάνιση αποτελεσμάτων Friedman Test
print("Friedman Test Results:")
print(friedman_result)

# Post-hoc ανάλυση (Dunn Test)
posthoc <- df %>%
  dunn_test(Error ~ Method, p.adjust.method = "bonferroni")

print("Post-hoc Analysis (Dunn Test):")
print(posthoc)

# Προσθήκη πληροφοριών post-hoc στο αρχικό dataset
stat_signif <- posthoc %>%
  filter(p.adj < 0.05) %>%
  mutate(y.position = seq(max(df$Error) + 2000,
                          max(df$Error) + 2000 * nrow(.),
                          by = 2000))

# Δημιουργία Boxplot με p-values
ggboxplot(df, x = "Method", y = "Error", color = "Method", add = "jitter",
          palette = "jco", add.params = list(size = 1.5)) +
  stat_pvalue_manual(stat_signif, label = "p.adj.signif",
                     tip.length = 0.02, bracket.size = 0.5) +
  labs(
    title = "Comparison of Optimization Methods",
    subtitle = "Friedman Test with Post-hoc Analysis",
    x = "",
    y = "Function Calls (Lower is Better)"
  ) +
  theme_minimal(base_size = 15) +
  theme(legend.position = "none") + # Αφαιρεί τον θρύλο
  scale_y_continuous(labels = comma) + # Διαχωρίζει τις τιμές στον y άξονα
     #Προσθέτουμε επεξήγηση για τα επίπεδα σημαντικότητας
  annotate("text", x = 2.8, y = 90000,
           label = paste0(

             "****: p < 0.0001 (Very extremely significant)"
           ),
           hjust = 0, size = 2, color = "blue")  # Μικρότερο μέγεθος κειμένου

