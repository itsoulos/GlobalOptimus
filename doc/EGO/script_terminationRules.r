library(tidyverse)
library(readxl)
library(ggpubr)
library(scales)
library(ggplot2)

dat <- read_excel("C:/Users/admin/Desktop/EGO/plot_ego.xlsx")



df <- dat |>
  rename(problem = 1) |>
  pivot_longer(-problem)



cmp <- list(c("UNIFORM", "KMEANS"))
scale_y_continuous(labels = label_number(prefix = "£", suffix = "k", scale = 1e-3, big.mark = ","))

options(scipen = 99999999)


ggboxplot(df, x = "name", y = "value", color = "name", shape = "name", add = "jitter")  +
  stat_compare_means(paired = TRUE, method = "t.test", comparisons = cmp) +
  stat_compare_means(paired = TRUE, label.y = 12000) +
  labs(x = "", y = "Function calls", shape = "", color = "", title = "", subtitle = "") +
  scale_y_continuous(labels = comma)



