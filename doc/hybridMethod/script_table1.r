library(tidyverse)
library(readxl)
library(ggpubr)
library(scales)
library(ggplot2)

dat <- read_excel("C:/Users/admin/Desktop/hybridMethod/table1.xlsx")



df <- dat |>
  rename(FUNCTION = 1) |>
  pivot_longer(-FUNCTION)



cmp <- list(c("DE", "PROPOSED"),c("IPSO", "PROPOSED"),c("GENETIC", "PROPOSED"))
scale_y_continuous(labels = label_number(prefix = "£", suffix = "k", scale = 1e-3, big.mark = ","))

options(scipen = 99999999)


ggboxplot(df, x = "name", y = "value", color = "name", shape = "name", add = "jitter")  +
  stat_compare_means(paired = TRUE, method = "t.test", comparisons = cmp) +
  stat_compare_means(paired = TRUE, label.y = 25000) +
  labs(x = "", y = "Function calls", shape = "", color = "", title = "", subtitle = "") +
  scale_y_continuous(labels = comma) +
  theme(axis.text.x = element_text(angle = 8, hjust = 1))

