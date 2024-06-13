library(tidyverse)
library(readxl)
library(ggpubr)

dat <- read_excel("~/Desktop/thanasis.xlsx")
df <- dat |>
  rename(problem = 1) |>
  pivot_longer(-problem) 

cmp <- list(c("PROPOSED", "MEAN"), c("PROPOSED", "DOUBLEBOX"), c("MEAN", "DOUBLEBOX"))
ggboxplot(df, x = "name", y = "value", color = "name", shape = "name", add = "jitter")  +
  stat_compare_means(paired = TRUE, method = "t.test", comparisons = cmp) +
  stat_compare_means(paired = TRUE, label.y = 100000) +
  labs(x = "method", y = "", shape = "", color = "", title = "", subtitle = "")
