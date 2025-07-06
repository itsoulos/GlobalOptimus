# Φόρτωση απαραίτητων βιβλιοθηκών
library(rstatix)    # Για στατιστικές δοκιμές (friedman_test(), dunn_test())
library(scales)     # Για μορφοποίηση αριθμών
library(tidyr)      # Για μετασχηματισμό δεδομένων (pivot_longer())
library(readxl)     # Για ανάγνωση αρχείων Excel
library(ggpubr)     # Για δημιουργία γραφημάτων
library(ggplot2)    # Για προσαρμογή γραφημάτων
library(dplyr)      # Για χειρισμό δεδομένων

# Εισαγωγή δεδομένων
dat <- read_excel("C:/Users/admin/Desktop/ECHO/echo.xlsx")

# Μετασχηματισμός δεδομένων σε μακρύ format
df <- dat %>%
  pivot_longer(cols = c(ECHO, "ECHO+APP", "ECHO+MEM", "ECHO+LTMA", "ECHO+APP+MEM"),
               names_to = "Method",
               values_to = "Error") %>%
  mutate(
    FUNCTION = as.factor(FUNCTION),
    Method = factor(Method, levels = c("ECHO", "ECHO+APP", "ECHO+MEM", "ECHO+LTMA", "ECHO+APP+MEM"))
  )

# 1. Friedman Test
friedman_result <- friedman_test(Error ~ Method | FUNCTION, data = df)

# 2. Υπολογισμός Κρίσιμης Διαφοράς (CD)
k <- nlevels(df$Method)      # Αριθμός μεθόδων (ομάδων)
N <- nlevels(df$FUNCTION)    # Αριθμός blocks (συναρτήσεων)
q_alpha <- qtukey(p = 0.95, nmeans = k, df = Inf)  # Κρίσιμη τιμή Tukey
CD <- q_alpha * sqrt((k*(k+1))/(6*N))              # Κρίσιμη Διαφορά

# 3. Post-hoc ανάλυση (Dunn Test) με Bonferroni διόρθωση
posthoc <- df %>%
  dunn_test(Error ~ Method, p.adjust.method = "bonferroni") %>%
  mutate(
    CD = CD,
    is_significant = ifelse(p.adj < 0.05, "Yes", "No"),
    significance_group = case_when(
      group2 == "ECHO" & is_significant == "Yes" ~ "Superior to ECHO",
      group1 == "ECHO" & is_significant == "Yes" ~ "Superior to ECHO",
      TRUE ~ "Not different"
    )
  )

# 4. Προετοιμασία δεδομένων για γράφημα
y_max <- max(df$Error)
y_step <- y_max * 0.1

stat_signif <- posthoc %>%
  mutate(
    y.position = y_max + (1:nrow(.)) * y_step,
    comparison = paste(group1, "vs", group2)
  )

# 5. Δημιουργία Boxplot με στατιστική σημαντικότητα
p <- ggboxplot(df, x = "Method", y = "Error", color = "Method",
               palette = "jco", add = "jitter") +
  stat_pvalue_manual(stat_signif,
                    label = "p.adj.signif",
                    tip.length = 0.01,
                    bracket.size = 0.3,
                    step.increase = 0.001) +
  labs(title = "Comparison of Optimization Methods",
       subtitle = "Friedman Test with Post-hoc Analysis",
       x = "",
       y = "Function calls") +
  theme_minimal(base_size = 15) +
  theme(legend.position = "none",
        axis.text.x = element_text(angle = 15, hjust = 0.5)) +
  scale_y_continuous(labels = comma, expand = expansion(mult = c(0.05, 0.2))) +

  annotate("text", x = 2.8, y = y_max * 2.3,
           label = "ns: p > 0.05 (Not significant)\n*: p < 0.05 (Very significant\n***: p < 0.001  (Εxtremely significant)\n****: p < 0.0001  (Very extremely significant)",
           hjust = 0, size = 3.5, color = "blue") +
  annotate("text", x = 0.9, y = y_max * 2.3,
           label = paste("Friedman Test: p =", signif(friedman_result$p, 3),
                        "\nCritical Difference =", round(CD, 3),
                        "\nCritical Value =", round(q_alpha, 4)),
           hjust = 0, size = 3.5, color = "blue")
# 6. Εκτύπωση αποτελεσμάτων
cat("===================== ΑΠΟΤΕΛΕΣΜΑΤΑ =====================\n")
cat("Friedman Test Results:\n")
print(friedman_result)
cat("\nCritical Difference Calculation:\n")
cat("Number of methods (k):", k, "\n")
cat("Number of blocks (N):", N, "\n")
cat("Critical value (q_alpha):", round(q_alpha, 4), "\n")
cat("Critical Difference (CD):", round(CD, 4), "\n\n")

cat("Post-hoc Analysis (Dunn Test) with Critical Difference:\n")
print(posthoc)

# 7. Εμφάνιση γραφήματος
print(p)

# 8. Αποθήκευση γραφήματος (προαιρετικό)
#ggsave("Friedman_Analysis.png", plot = p, width = 10, height = 8, dpi = 300)
