library(tidyverse)
library(readxl)
library(ggpubr)
library(scales)
library(ggplot2)

dat <- read_excel("C:/Users/admin/Desktop/NNC_IMPROVED/table1.xlsx")



df <- dat |>
  rename(DATASET = 1) |>
  pivot_longer(-DATASET)



cmp <- list(c("BFGS", "INNC"),c("GENETIC", "INNC"),c("NNC", "INNC"))
scale_y_continuous(labels = label_number(prefix = "£", suffix = "k", scale = 1e-3, big.mark = ","))

options(scipen = 99999999)


ggboxplot(df, x = "name", y = "value", color = "name", shape = "name", add = "jitter")  +
  stat_compare_means(paired = TRUE, method = "t.test", comparisons = cmp) +
  stat_compare_means(paired = TRUE, label.y = 100) +
  labs(x = "", y = "Classification error (%)", shape = "", color = "", title = "", subtitle = "") +
  scale_y_continuous(labels = comma)



