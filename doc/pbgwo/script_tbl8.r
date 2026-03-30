library(tidyverse)
library(readxl)
library(scales)

# ================== CONFIG ==================
excel_path <- "C:/Users/admin/Desktop/pbgwo/tbl8.xlsx"
out_dir    <- "C:/Users/admin/Desktop/pbgwo/out_tbl8"
dir.create(out_dir, showWarnings = FALSE, recursive = TRUE)

# ================== HELPERS ==================
format_p_label <- function(p) {
  if (is.na(p) || !is.finite(p)) return("p = NA")
  if (p < 2.2e-16) return("p < 2.2e-16")
  if (p < 1e-3) return(paste0("p = ", format(p, scientific = TRUE, digits = 3)))
  paste0("p = ", formatC(p, format = "f", digits = 4))
}

friedman_label_text <- function(p) {
  if (is.na(p) || !is.finite(p)) return("Overall Friedman test: p = NA")
  if (p < 2.2e-16) return("Overall Friedman test: p < 2.2e-16")
  if (p < 1e-3) return(paste0("Overall Friedman test: p = ", format(p, scientific = TRUE, digits = 3)))
  paste0("Overall Friedman test: p = ", formatC(p, format = "f", digits = 4))
}

safe_write_csv <- function(df, path) {
  try(write.csv(df, path, row.names = FALSE), silent = TRUE)
}

# Direct Nemenyi pairwise p-values from average rank differences
nemenyi_pairs <- function(common_df, method_levels) {
  d <- common_df |>
    mutate(Method = factor(Method, levels = method_levels)) |>
    arrange(DATASET, Method)

  rank_df <- d |>
    group_by(DATASET) |>
    mutate(Rank = rank(Error, ties.method = "average")) |>
    ungroup()

  avg_ranks <- rank_df |>
    group_by(Method) |>
    summarise(avg_rank = mean(Rank, na.rm = TRUE), .groups = "drop") |>
    mutate(Method = as.character(Method))

  k <- length(method_levels)
  n <- length(unique(rank_df$DATASET))

  if (k < 2 || n < 2) {
    return(list(
      avg_ranks = avg_ranks,
      pairs = tibble()
    ))
  }

  se <- sqrt(k * (k + 1) / (6 * n))

  cmb <- combn(method_levels, 2, simplify = FALSE)

  pairs_tbl <- map_dfr(cmb, function(z) {
    g1 <- z[1]
    g2 <- z[2]

    r1 <- avg_ranks$avg_rank[match(g1, avg_ranks$Method)]
    r2 <- avg_ranks$avg_rank[match(g2, avg_ranks$Method)]

    diff_rank <- abs(r1 - r2)
    q_stat <- diff_rank / se

    # Correct Nemenyi p-value
    p_val <- ptukey(q_stat * sqrt(2), nmeans = k, df = Inf, lower.tail = FALSE)

    tibble(
      group1 = g1,
      group2 = g2,
      avg_rank_1 = r1,
      avg_rank_2 = r2,
      rank_diff  = diff_rank,
      q_stat     = q_stat,
      p_value    = p_val
    )
  })

  list(
    avg_ranks = avg_ranks,
    pairs = pairs_tbl
  )
}

# ================== 1) READ & CLEAN ==================
dat <- read_excel(excel_path)

if (ncol(dat) < 2) {
  message("The Excel file does not contain enough columns.")
  quit(save = "no")
}

names(dat)[1] <- "DATASET"
header_methods <- trimws(names(dat)[-1])

df0 <- dat |>
  pivot_longer(-DATASET, names_to = "Method", values_to = "ErrorRaw") |>
  mutate(
    DATASET = as.character(DATASET),
    Method  = trimws(as.character(Method)),
    Error   = suppressWarnings(as.numeric(ErrorRaw))
  ) |>
  filter(!is.na(DATASET), !is.na(Method), !is.na(Error), is.finite(Error))

if (nrow(df0) == 0) {
  message("No valid numeric data were found in the Excel file.")
  quit(save = "no")
}

# mean per DATASET x Method if duplicates exist
df1 <- df0 |>
  group_by(DATASET, Method) |>
  summarise(Error = mean(Error, na.rm = TRUE), .groups = "drop")

present_methods <- header_methods[header_methods %in% unique(df1$Method)]
if (length(present_methods) == 0) {
  present_methods <- unique(df1$Method)
}

df1 <- df1 |>
  filter(Method %in% present_methods) |>
  mutate(Method = factor(Method, levels = present_methods))

# ================== 2) KEEP ONLY COMPLETE BLOCKS ==================
common_sets <- df1 |>
  group_by(DATASET) |>
  summarise(nm = n_distinct(Method), .groups = "drop") |>
  filter(nm == length(present_methods)) |>
  pull(DATASET)

common_df <- df1 |>
  filter(DATASET %in% common_sets) |>
  as.data.frame()

if (nrow(common_df) > 0) {
  common_df$DATASET <- factor(common_df$DATASET)
  common_df$Method  <- factor(common_df$Method, levels = present_methods)
}

plot_df <- if (nrow(common_df) > 0) common_df else as.data.frame(df1)

# ================== 3) FRIEDMAN ==================
friedman_p <- NA_real_
friedman_ok <- FALSE

if (length(unique(common_df$Method)) >= 2 && length(unique(common_df$DATASET)) >= 2) {
  fr <- try(suppressWarnings(friedman.test(Error ~ Method | DATASET, data = common_df)), silent = TRUE)
  if (!inherits(fr, "try-error")) {
    friedman_p <- fr$p.value
    friedman_ok <- TRUE
  }
}

# ================== 4) NEMENYI POST-HOC ==================
pairwise_results <- tibble()
avg_ranks_tbl <- tibble()

if (friedman_ok) {
  nem <- nemenyi_pairs(common_df, present_methods)
  avg_ranks_tbl <- nem$avg_ranks
  pairwise_results <- nem$pairs |>
    mutate(
      p_lab = vapply(p_value, format_p_label, character(1)),
      x1    = match(group1, present_methods),
      x2    = match(group2, present_methods),
      xmin  = pmin(x1, x2),
      xmax  = pmax(x1, x2),
      xmid  = (xmin + xmax) / 2
    ) |>
    arrange(xmax - xmin, xmin, xmax)
}

# ================== 5) Y POSITIONS ==================
y_min_data <- suppressWarnings(min(plot_df$Error, na.rm = TRUE))
y_max_data <- suppressWarnings(max(plot_df$Error, na.rm = TRUE))

if (!is.finite(y_min_data)) y_min_data <- 0
if (!is.finite(y_max_data)) y_max_data <- 1

data_span <- y_max_data - y_min_data
if (!is.finite(data_span) || data_span <= 0) data_span <- max(abs(y_max_data), 1)

step_y <- max(0.15 * data_span, 0.8)

if (nrow(pairwise_results) > 0) {
  pairwise_results <- pairwise_results |>
    mutate(y = y_max_data + seq_len(n()) * step_y)

  global_ymax <- max(pairwise_results$y, na.rm = TRUE) + 2 * step_y
} else {
  global_ymax <- y_max_data + 3 * step_y
}

global_ymin <- min(0, y_min_data - 0.05 * data_span)

# ================== 6) PLOT ==================
shape_vals <- c(16, 17, 15, 18, 3, 4, 8, 7, 9, 10, 12, 13)
shape_vals <- rep(shape_vals, length.out = length(present_methods))

subtitle_txt <- if (friedman_ok) {
  "Friedman test with Nemenyi post-hoc"
} else {
  "Descriptive comparison (insufficient complete blocks for Friedman/Nemenyi)"
}

p <- ggplot(plot_df, aes(x = Method, y = Error, color = Method)) +
  geom_boxplot(width = 0.75, outlier.shape = NA) +
  geom_jitter(aes(shape = Method), width = 0.15, size = 1.5, alpha = 0.85) +
  scale_shape_manual(values = shape_vals) +
  scale_y_continuous(
    labels = comma,
    limits = c(global_ymin, global_ymax),
    expand = expansion(mult = c(0.02, 0.05))
  ) +
  labs(
    x = "",
    y = "Classification error [%]",
    title = "Model Comparison",
    subtitle = subtitle_txt
  ) +
  theme_minimal(base_size = 15) +
  theme(
    legend.position = "none",
    axis.line = element_line(color = "blue"),
    axis.ticks = element_line(color = "black"),
    axis.text.x = element_text(color = "blue", angle = 35, hjust = 1, vjust = 1),
    axis.text.y = element_text(color = "blue"),
    panel.grid.major = element_line(color = "grey70", linewidth = 0.5),
    panel.grid.minor = element_blank()
  )

if (nrow(pairwise_results) > 0) {
  p <- p +
    geom_segment(
      data = pairwise_results,
      aes(x = xmin, xend = xmax, y = y, yend = y),
      inherit.aes = FALSE,
      linewidth = 0.6,
      color = "black"
    ) +
    geom_segment(
      data = pairwise_results,
      aes(x = xmin, xend = xmin, y = y, yend = y - 0.35 * step_y),
      inherit.aes = FALSE,
      linewidth = 0.6,
      color = "black"
    ) +
    geom_segment(
      data = pairwise_results,
      aes(x = xmax, xend = xmax, y = y, yend = y - 0.35 * step_y),
      inherit.aes = FALSE,
      linewidth = 0.6,
      color = "black"
    ) +
    geom_text(
      data = pairwise_results,
      aes(x = xmid, y = y + 0.12 * step_y, label = p_lab),
      inherit.aes = FALSE,
      size = 3.2,
      vjust = 0,
      color = "black"
    )
}

if (friedman_ok) {
  p <- p +
    annotate(
      "text",
      x = max(1, ceiling(length(present_methods) / 2)),
      y = global_ymax - 0.1 * step_y,
      label = friedman_label_text(friedman_p),
      size = 4,
      vjust = 1,
      color = "black"
    )
}

print(p)

# ================== 7) SAVE ==================
png_path <- file.path(out_dir, "tbl8_one_plot_all_methods.png")
pdf_path <- file.path(out_dir, "tbl8_one_plot_all_methods.pdf")

ggsave(png_path, plot = p, width = 12, height = 9, dpi = 300)
ggsave(pdf_path, plot = p, width = 12, height = 9)

safe_write_csv(df0, file.path(out_dir, "tbl8_long_raw_cleaned.csv"))
safe_write_csv(df1, file.path(out_dir, "tbl8_dataset_method_means.csv"))
safe_write_csv(common_df, file.path(out_dir, "tbl8_common_datasets_only.csv"))
safe_write_csv(avg_ranks_tbl, file.path(out_dir, "tbl8_average_ranks.csv"))
safe_write_csv(pairwise_results, file.path(out_dir, "tbl8_pairwise_nemenyi.csv"))

# ================== 8) CONSOLE OUTPUT ==================
message("Methods present (ordered): ", paste(present_methods, collapse = ", "))
message("Datasets used in plot: ", length(unique(plot_df$DATASET)))
message("Common DATASETS used for Friedman/Nemenyi: ", length(unique(common_df$DATASET)))

if (friedman_ok) {
  message("Overall Friedman p = ", friedman_p)
} else {
  message("Friedman/Nemenyi skipped because complete blocks were not sufficient.")
}

if (nrow(avg_ranks_tbl) > 0) {
  message("Average ranks:")
  print(avg_ranks_tbl)
}

if (nrow(pairwise_results) > 0) {
  message("Pairwise Nemenyi results:")
  print(pairwise_results |> select(group1, group2, avg_rank_1, avg_rank_2, rank_diff, p_value))
} else {
  message("No pairwise Nemenyi results were produced.")
}

message("Saved plot PNG: ", png_path)
message("Saved plot PDF: ", pdf_path)
message("Done.")
