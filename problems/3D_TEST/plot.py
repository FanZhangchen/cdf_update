import pandas as pd
import matplotlib.pyplot as plt

# Load your CSV file (adjust path if needed)
df = pd.read_csv('data_output_111.csv')

# Assuming the columns are named exactly as shown in the image
plt.plot(df['strain_xx'], df['stress_xx'])  

# Customize the plot (add labels, title, etc.)
plt.xlabel('Strain XX')
plt.ylabel('Stress XX')
plt.title('Stress-Strain Curve')
# plt.grid(True)

# Show the plot
plt.show()
