library(tidyverse)
library(readxl)
library(ggpubr)
library(scales)
library(ggplot2)

dat <- read_excel("C:/Users/admin/Desktop/NNC_IMPROVED/table4.xlsx")



df <- dat |>
  rename(DATASET = 1) |>
  pivot_longer(-DATASET)



cmp <- list(c("INNC-Ni=5", "INNC-Ni=10"),c("INNC-Ni=5", "INNC-Ni=20"),c("INNC-Ni=10", "INNC-Ni=20"))
scale_y_continuous(labels = label_number(prefix = "£", suffix = "k", scale = 1e-3, big.mark = ","))

options(scipen = 99999999)


ggboxplot(df, x = "name", y = "value", color = "name", shape = "name", add = "jitter")  +
  stat_compare_means(paired = TRUE, method = "t.test", comparisons = cmp) +
  stat_compare_means(paired = TRUE, label.y = 90) +
  labs(x = "", y = "Regression error", shape = "", color = "", title = "", subtitle = "") +
  scale_y_continuous(labels = comma)



