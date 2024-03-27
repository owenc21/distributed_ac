import matplotlib.pyplot as plt
import numpy as np

def plot_data(data, output_file, x_axis, y_axis, plot_title):
  """Plots the data in a text file and saves the plot to the specified output file.

  Args:
    data: A list of integers representing the data to be plotted.
    output_file: The path to the output file where the plot will be saved.
  """

  # Convert the data to a NumPy array
  data_array = np.asarray(data)

  # Create the plot
  plt.plot(data_array)

  # Set the title and axis labels
  plt.title(plot_title)
  plt.xlabel(x_axis)
  plt.ylabel(y_axis)

  # Save the plot to the output file
  plt.savefig(output_file)

if __name__ == "__main__":
  # Get the input file path from the user
  input_file = input("Enter the path to the text file: ")
  x_axis = input("Enter x axis name: ")
  y_axis = input("Enter y axis name: ")
  plot_title = input("Enter plot title: ")
  
  with open(input_file, 'r') as f:
	data = [int(line.strip()) for line in f]

  output_file = input("Enter the path to save the plot: ")


  plot_data(data, output_file, x_axis, y_axis, plot_title)

  print(f"Plot saved to: {output_file}")
