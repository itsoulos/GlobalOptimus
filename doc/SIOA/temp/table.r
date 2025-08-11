# Load libraries
library(readxl)
library(tidyr)
library(dplyr)
library(ggplot2)
library(scales)  # For label_comma()

# Read the Excel file
file_path <- "C:/Users/admin/Desktop/SIOA/pop100.xlsx"
data <- read_excel(file_path)

# Reshape data from wide to long format for plotting
data_long <- data %>%
  pivot_longer(cols = -FUNCTION, names_to = "Model", values_to = "FunctionCalls")

# Custom formatting function
custom_format <- function(x) {
  case_when(
    x >= 10000 ~ comma(x),      # Format with comma for 5+ digits
    TRUE ~ as.character(x)      # No comma for 4 digits or less
  )
}

# Create a dot plot
plot_dot <- ggplot(data_long, aes(x = Model, y = FunctionCalls, color = Model)) +
  geom_point(size = 3, position = position_jitter(width = 0.1, height = 0)) +
  facet_wrap(~FUNCTION, scales = "free_y") +  # Separate panels for each dataset
  scale_y_continuous(labels = custom_format) +  # Apply custom formatting
  labs(
    title = "Comparison of Methods",
    subtitle = "Each problem is displayed in a separate panel",
    x = "",
    y = "Function calls",
    color = "Model"
  ) +
  theme_minimal() +
  theme(
    axis.text.x = element_text(angle = 45, hjust = 1),
    plot.title = element_text(face = "bold", size = 14),
    plot.subtitle = element_text(size = 12),
    legend.position = "none"  # Remove redundant legend
  )

# Display the plot
print(plot_dot)

# Save the plot to a file
ggsave("dot_plot_comparison.png", plot_dot, width = 14, height = 10)
