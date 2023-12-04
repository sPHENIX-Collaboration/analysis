# Importing necessary libraries
import tkinter as tk  # For creating GUI elements
from tkinter import ttk, messagebox, filedialog, simpledialog  # Additional GUI components and dialogs
import pandas as pd  # For data handling and manipulation
import matplotlib  # For plotting graphs
matplotlib.use('TkAgg')  # Setting the backend of matplotlib to TkAgg for GUI integration
import matplotlib.pyplot as plt  # For creating and manipulating plots
from matplotlib.lines import Line2D  # For creating line objects in matplotlib
from matplotlib.container import ErrorbarContainer  # For handling error bars in plots
from matplotlib.legend_handler import HandlerErrorbar  # For custom legend handling with error bars
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2Tk  # Tkinter integration with matplotlib
import sys  # For system-specific parameters and functions



# Reading data from a CSV file into a pandas DataFrame
data = pd.read_csv('/Users/patsfan753/Desktop/Desktop/AnalyzePi0s_Final/dataOutput/PlotByPlotOutput.csv')

# Dictionary to maintain the visibility state of different centrality categories in the plot
centrality_visibility = {"60-100%": True, "40-60%": True, "20-40%": True, "0-20%": True}

# List of labels corresponding to different centrality categories
labels = ["60-100%", "40-60%", "20-40%", "0-20%"]

# Global variables to hold the maximum Y-values for different plot types; used for scaling the plot
global_max_y_signalYield = 0
global_max_y_GaussianMean = 0
global_min_y_GaussianMean = 0 #needed for a buffer for low gaussian mean values
global_max_y_GaussianSigma = 0
global_max_y_RelativeSignalError = 0

# Counters for the total number of points plotted in different plot types
total_point_count_SignalYield = 0
total_point_count_GaussianMean = 0
total_point_count_GaussianSigma = 0
total_point_count_RelativeSignalError = 0

# Lists to store containers (plot elements) for different plot types
errorbar_containers_SignalYield = []
errorbar_containers_GaussianMean = []
errorbar_containers_GaussianSigma = []

# Dictionaries to store the history of plotted data points for each plot type
plot_SignalYield_history = {}
plot_GaussianMean_history = {}
plot_GaussianSigma_history = {}
plot_RelativeSignalError_history = {}

# Counters to keep track of the number of updates made to each plot type
plot_update_count_SignalYield = 0
plot_update_count_GaussianMean = 0
plot_update_count_GaussianSigma = 0
plot_update_count_RelativeSignalError = 0

# List of colors used for differentiating data points in the plot based on centrality categories
colors = ['red', 'blue', 'lightgreen', 'deeppink']

# To initalize plot type choice thats outputted before GUI box
current_plot_type = "SignalYield"

#initialize the legend for a plot, setting up the labels, colors, and markers for different centrality ranges
def init_legend():
    # Define labels for the legend. These correspond to different centrality categories.
    labels = ["60-100%", "40-60%", "20-40%", "0-20%"]
    
    # Define colors for each centrality category. These colors will be used in the plot.
    colors = ['red', 'blue', 'lightgreen', 'deeppink']
    
    # Define marker styles for each centrality category.
    markers = ['o', 's', '^', 'v']
    
    # List to hold dummy line objects. These are used for creating the legend entries.
    dummy_lines = []
    
    # Loop over each combination of color, marker, and label.
    for color, marker, label in zip(colors, markers, labels):
        # Create an errorbar object with no data but with the specified color, marker, and label.
        # This is used for the legend entry, not for the actual plot data.
        line = ax.errorbar([], [], yerr=[], fmt=marker, color=color, label=label)
        
        # Append the created dummy line object to the list for use in the legend.
        dummy_lines.append(line)
        
    # Define a custom handler map for the legend. This specifies how error bars should be handled.
    # In this case, the error bar size in the legend is set to 0.5.
    handler_map = {ErrorbarContainer: HandlerErrorbar(yerr_size=0.5)}
    
    # Determine the location of the legend based on the current plot type.
    # If the current plot type is "RelativeSignalError", place the legend in the upper left.
    # Otherwise, place it in the upper right.
    if current_plot_type == "RelativeSignalError":
        legend_loc = 'upper left'
    else:
        legend_loc = 'upper right'
        
    # Create the legend on the plot.
    # The legend uses the dummy lines, the specified title, the custom handler map, and the determined location.
    ax.legend(handles=dummy_lines, title='Centrality:', handler_map=handler_map, loc = legend_loc)
    
#create interactive plot for signal yield data
def plot_SignalYield(filtered_data, clear_plot):
    # Declare global variables to be modified inside this function.
    global global_max_y_signalYield, plot_SignalYield_history, plot_update_count_SignalYield, errorbar_containers_SignalYield, total_point_count_SignalYield, centrality_visibility
    
    # Check if the plot should be cleared before drawing new data.
    if clear_plot:
        ax.clear()  # Clear the current axes.
        ax.set_yscale('log')  # Set y-axis scale to logarithmic.
        global_max_y_signalYield = 0  # Reset the maximum y-value for signal yield.
        init_legend()  # Initialize the legend for the plot.
        errorbar_containers_SignalYield.clear()  # Clear any existing error bars.
        plot_SignalYield_history = {}  # Reset the history of plotted signal yields.
        plot_update_count_SignalYield = 0  # Reset the count of plot updates.
    else:
        plot_update_count_SignalYield += 1  # Increment the count for each new overlay.

    # Define base x-coordinates for the plot points.
    x_points_base = [2.5, 3.5, 4.5]

    # Set x-axis ticks.
    x_ticks = [2, 3, 4, 5]

    # Define markers for the plot points.
    markers = ['o', 's', '^', 'v']

    # Variable to track the highest y-value in the plot.
    local_max_y = 0

    # Gather current filter/cut values from input fields.
    current_cuts = {
        'E': energy_entry.get(),
        'A': asymmetry_entry.get(),
        'C': chi2_entry.get(),
        'D': delta_r_entry.get()
    }
    
    # Iterate over each centrality category.
    for i, (color, marker) in enumerate(zip(colors, markers)):
        # Check visibility setting for each centrality category.
        if not centrality_visibility[labels[i]]:
            continue  # Skip hidden categories.

        # Calculate offsets for x-values to avoid overlapping points.
        base_jitter_offset = (i - 1.5) * 0.05
        overlay_jitter_offset = plot_update_count_SignalYield * 0.08
        total_jitter_offset = base_jitter_offset + overlay_jitter_offset

        # Determine the indices for the current centrality category.
        indices = range(i * 3, (i + 1) * 3)

        # Calculate x-values, y-values, error values, and S/B ratios for each point.
        x_vals = [x + total_jitter_offset for x in x_points_base]
        y_vals = [filtered_data.iloc[j]['Yield'] for j in indices]
        y_errs = [filtered_data.iloc[j]['YieldError'] for j in indices]
        S_B_ratios = [filtered_data.iloc[j]['S/B'] for j in indices]

        # Plot each data point with the corresponding error bar.
        for j, (x_val, y_val, y_err, S_B_ratio) in enumerate(zip(x_vals, y_vals, y_errs, S_B_ratios)):
            index = i * 3 + j  # Calculate the specific index for the point.
            point_id = (plot_update_count_SignalYield, i, j)  # Unique identifier for each point.
            
            # Store data about the point in the history dictionary.
            plot_SignalYield_history[point_id] = {
                'y_val': y_val, 'y_err': y_err, 'cuts': current_cuts.copy(),
                'centrality': labels[i], 'csv_index': index, 'S_B_ratio': S_B_ratio
            }

            # Create an error bar container for each point and add it to the list.
            container = ax.errorbar(x_val, y_val, yerr=y_err, fmt=marker, color=color)
            container[0].set_gid(point_id)  # Set a group ID for the container.
            container[0].set_picker(5)  # Enable picking on the plot point.
            errorbar_containers_SignalYield.append(container)

            # Increment the count of total points plotted.
            total_point_count_SignalYield += 1
            
        # Update local max y-value based on the plotted data.
        local_max_y = max(local_max_y, max(y_vals) * 10)
        
    # Update global max y-value if the local max is greater.
    if local_max_y > global_max_y_signalYield:
        global_max_y_signalYield = local_max_y
        
    #Set plot limits/labels
    ax.set_ylim(0.1, global_max_y_signalYield)
    ax.set_xticks(x_ticks)
    ax.set_xlim(1.5, 5.5)
    ax.set_title(r"$\pi^0$ Yield")
    ax.set_xlabel(r"$\pi^0$ pT")
    ax.set_ylabel(r"$\pi^0$ Signal Yield")
    
    #Redraw canvas with new data
    canvas.draw()
    
#Interactive plot of gaussian mean data filled with gaussian mean error
def plot_GaussianMean(filtered_data, clear_plot):
    # Access global variables to modify within the function.
    global global_max_y_GaussianMean, global_min_y_GaussianMean, plot_GaussianMean_history, plot_update_count_GaussianMean, errorbar_containers_GaussianMean, total_point_count_GaussianMean, centrality_visibility

    # Clear the plot and reset related variables if clear_plot is True.
    if clear_plot:
        ax.clear()  # Clears the current axes.
        global_max_y_GaussianMean = 0  # Resets the maximum y-value for Gaussian Mean.
        global_min_y_GaussianMean = 0  # Resets the minimum y-value for Gaussian Mean.
        init_legend()  # Calls function to initialize the plot's legend.
        errorbar_containers_GaussianMean.clear()  # Clears any existing error bars from the plot.
        plot_GaussianMean_history = {}  # Resets the history of Gaussian Mean data.
        plot_update_count_GaussianMean = 0  # Resets the count of updates/overlays on the plot.
    else:
        plot_update_count_GaussianMean += 1  # Increments the count for each overlay.

    # Define base x-coordinates for the plot points.
    x_points_base = [2.5, 3.5, 4.5]

    # Set the x-axis ticks for the plot.
    x_ticks = [2, 3, 4, 5]

    # Define markers for each plot point.
    markers = ['o', 's', '^', 'v']

    # Variables to keep track of the highest and lowest y-values in the plot.
    local_max_y = 0
    local_min_y = 0

    # Gather current filter/cut values from user input fields.
    current_cuts = {
        'E': energy_entry.get(),
        'A': asymmetry_entry.get(),
        'C': chi2_entry.get(),
        'D': delta_r_entry.get()
    }

    # Iterate over each centrality category for plotting.
    for i, (color, marker) in enumerate(zip(colors, markers)):
        # Skip plotting for categories that are not visible.
        if not centrality_visibility[labels[i]]:
            continue

        # Calculate x-value offsets to prevent point overlapping.
        base_jitter_offset = (i - 1.5) * 0.05
        overlay_jitter_offset = plot_update_count_GaussianMean * 0.08
        total_jitter_offset = base_jitter_offset + overlay_jitter_offset

        # Determine the indices for data points in the current category.
        indices = range(i * 3, (i + 1) * 3)

        # Calculate x-values, mean values, errors, and S/B ratios for each point.
        x_vals = [x + total_jitter_offset for x in x_points_base]
        mean_vals = [filtered_data.iloc[j]['GaussMean'] for j in indices]
        mean_errs = [filtered_data.iloc[j]['GaussMeanError'] for j in indices]
        S_B_ratios = [filtered_data.iloc[j]['S/B'] for j in indices]

        # Plot each data point with the corresponding error bar.
        for j, (x_val, mean_val, mean_err, S_B_ratio) in enumerate(zip(x_vals, mean_vals, mean_errs, S_B_ratios)):
            index = i * 3 + j  # Calculate the specific index for each point.
            point_id = (plot_update_count_GaussianMean, i, j)  # Unique identifier for the point.
            
            # Store point data in the history dictionary.
            plot_GaussianMean_history[point_id] = {
                'mean_val': mean_val, 'mean_err': mean_err, 'cuts': current_cuts.copy(),
                'centrality': labels[i], 'csv_index': index, 'S_B_ratio': S_B_ratio
            }

            # Create an error bar container for the point and add it to the list.
            container = ax.errorbar(x_val, mean_val, yerr=mean_err, fmt=marker, color=color)
            container[0].set_gid(point_id)  # Assign a group ID to the container.
            container[0].set_picker(5)  # Enable picking on the plot point.
            errorbar_containers_GaussianMean.append(container)

            # Update the total point count.
            total_point_count_GaussianMean += 1

        # Update the local maximum and minimum y-values based on plotted data.
        max_y_val_with_err = max(mean_val + mean_err for mean_val, mean_err in zip(mean_vals, mean_errs))
        min_y_val_with_err = min(mean_val - mean_err for mean_val, mean_err in zip(mean_vals, mean_errs))
        local_max_y = max(local_max_y, max_y_val_with_err)
        local_min_y = min(local_min_y, min_y_val_with_err)

    # Update global maximum and minimum y-values for Gaussian Mean.
    if local_max_y > global_max_y_GaussianMean:
        global_max_y_GaussianMean = local_max_y
    if local_min_y < global_min_y_GaussianMean:
        global_min_y_GaussianMean = local_min_y
    else:
        global_min_y_GaussianSigma = 0  # Set a default lower bound.

    # Set y-axis limits with buffer factors for better visualization.
    upper_buffer_factor = 1.5
    lower_buffer_factor = 0.1
    ax.set_ylim(global_min_y_GaussianMean - (global_min_y_GaussianMean * lower_buffer_factor),
                global_max_y_GaussianMean * upper_buffer_factor)

    # Highlight the theoretical pi0 mass with a horizontal line.
    pi0_mass = 0.135
    ax.axhline(y=pi0_mass, color='gray', linestyle='--')
    ax.text(1.6, pi0_mass, r'$\pi^0$ mass', va='center', ha='left', backgroundcolor='white')

    # Set x-axis properties.
    ax.set_xticks(x_ticks)
    ax.set_xlim(1.5, 5.5)

    # Add titles and labels to the plot.
    ax.set_title(r"Gaussian Mean Values vs $\pi^0$ pT")
    ax.set_xlabel(r"$\pi^0$ pT")
    ax.set_ylabel(r"Gaussian Mean")

    # Redraw the canvas with the updated plot.
    canvas.draw()


def plot_GaussianSigma(filtered_data, clear_plot):
    # Access and modify global variables within the function.
    global global_max_y_GaussianSigma, global_min_y_GaussianSigma, plot_GaussianSigma_history, plot_update_count_GaussianSigma, errorbar_containers_GaussianSigma, total_point_count_GaussianSigma, centrality_visibility

    # Clear the existing plot and reset variables if clear_plot is True.
    if clear_plot:
        ax.clear()  # Clears the current axes for a new plot.
        global_max_y_GaussianSigma = 0  # Resets the maximum y-value for Gaussian Sigma.
        global_min_y_GaussianSigma = 0  # Resets the minimum y-value for Gaussian Sigma.
        init_legend()  # Initializes the legend for the plot.
        errorbar_containers_GaussianSigma.clear()  # Clears existing error bars.
        plot_GaussianSigma_history = {}  # Resets the history for Gaussian Sigma data.
        plot_update_count_GaussianSigma = 0  # Resets the overlay count.
    else:
        plot_update_count_GaussianSigma += 1  # Increment overlay count for each update.

    # Define base x-coordinates for plotting.
    x_points_base = [2.5, 3.5, 4.5]

    # Set x-axis ticks for the plot.
    x_ticks = [2, 3, 4, 5]

    # Define markers for plotting points.
    markers = ['o', 's', '^', 'v']

    # Initialize variables to track the highest and lowest y-values.
    local_max_y = 0
    local_min_y = 0

    # Retrieve current filter/cut values from the interface.
    current_cuts = {
        'E': energy_entry.get(),
        'A': asymmetry_entry.get(),
        'C': chi2_entry.get(),
        'D': delta_r_entry.get()
    }

    # Iterate through each centrality category for plotting.
    for i, (color, marker) in enumerate(zip(colors, markers)):
        # Skip plotting if the centrality category is not visible.
        if not centrality_visibility[labels[i]]:
            continue

        # Calculate offsets to avoid point overlapping.
        base_jitter_offset = (i - 1.5) * 0.05
        overlay_jitter_offset = plot_update_count_GaussianSigma * 0.08
        total_jitter_offset = base_jitter_offset + overlay_jitter_offset

        # Determine indices for data points in this category.
        indices = range(i * 3, (i + 1) * 3)

        # Calculate x-values, sigma values, errors, and S/B ratios for each point.
        x_vals = [x + total_jitter_offset for x in x_points_base]
        sigma_vals = [filtered_data.iloc[j]['GaussSigma'] for j in indices]
        sigma_errs = [filtered_data.iloc[j]['GaussSigmaError'] for j in indices]
        S_B_ratios = [filtered_data.iloc[j]['S/B'] for j in indices]

        # Plot each data point with an error bar.
        for j, (x_val, sigma_val, sigma_err, S_B_ratio) in enumerate(zip(x_vals, sigma_vals, sigma_errs, S_B_ratios)):
            index = i * 3 + j  # Determine the specific index for each point.
            point_id = (plot_update_count_GaussianSigma, i, j)  # Create a unique identifier for the point.
            
            # Store point data in the history dictionary.
            plot_GaussianSigma_history[point_id] = {
                'sigma_val': sigma_val, 'sigma_err': sigma_err, 'cuts': current_cuts.copy(),
                'centrality': labels[i], 'csv_index': index, 'S_B_ratio': S_B_ratio
            }

            # Create an error bar container for the point and add it to the list.
            container = ax.errorbar(x_val, sigma_val, yerr=sigma_err, fmt=marker, color=color)
            container[0].set_gid(point_id)  # Assign group ID to the container.
            container[0].set_picker(5)  # Enable picking on the plot point.
            errorbar_containers_GaussianSigma.append(container)

            # Increment the total point count.
            total_point_count_GaussianSigma += 1

        # Update local maximum and minimum y-values based on plotted data.
        max_y_val_with_err = max(sigma_val + sigma_err for sigma_val, sigma_err in zip(sigma_vals, sigma_errs))
        min_y_val_with_err = min(sigma_val - sigma_err for sigma_val, sigma_err in zip(sigma_vals, sigma_errs))
        local_max_y = max(local_max_y, max_y_val_with_err)
        local_min_y = min(local_min_y, min_y_val_with_err)

    # Update global maximum and minimum y-values for Gaussian Sigma.
    if local_max_y > global_max_y_GaussianSigma:
        global_max_y_GaussianSigma = local_max_y
    if local_min_y < global_min_y_GaussianSigma:
        global_min_y_GaussianSigma = local_min_y
    else:
        global_min_y_GaussianSigma = 0  # Default lower bound.

    # Set y-axis limits with buffer factors for better visualization.
    upper_buffer_factor = 1.5
    lower_buffer_factor = 0.1
    ax.set_ylim(global_min_y_GaussianSigma - (global_min_y_GaussianSigma * lower_buffer_factor),
                global_max_y_GaussianSigma * upper_buffer_factor)

    # Set x-axis properties.
    ax.set_xticks(x_ticks)
    ax.set_xlim(1.5, 5.5)

    # Add titles and labels to the plot.
    ax.set_title(r"Gaussian Sigma Values vs $\pi^0$ pT")
    ax.set_xlabel(r"$\pi^0$ pT")
    ax.set_ylabel(r"Gaussian Sigma")

    # Redraw the canvas with the updated plot.
    canvas.draw()

# Global dictionary to map each plotted point to its line index on the plot.
line_index_mapping = {}
def plot_RelativeSignalError(filtered_data, clear_plot):
    # Access and modify global variables within the function.
    global global_max_y_RelativeSignalError, plot_RelativeSignalError_history, plot_update_count_RelativeSignalError, total_point_count_RelativeSignalError, centrality_visibility, line_index_mapping
    
    # Clear the plot and reset relevant variables if clear_plot is True.
    if clear_plot:
        ax.clear()  # Clears the current axes for a new plot.
        global_max_y_RelativeSignalError = 0  # Resets the maximum y-value for Relative Signal Error.
        init_legend()  # Initializes the legend for the plot.
        plot_RelativeSignalError_history = {}  # Resets the history for Relative Signal Error data.
        plot_update_count_RelativeSignalError = 0  # Resets the overlay count.
    else:
        plot_update_count_RelativeSignalError += 1  # Increment overlay count for each update.

    # Define base x-coordinates for plotting.
    x_points_base = [2.5, 3.5, 4.5]

    # Set x-axis ticks for the plot.
    x_ticks = [2, 3, 4, 5]

    # Define markers for plotting points.
    markers = ['o', 's', '^', 'v']

    # Initialize a variable to track the highest y-value.
    local_max_y = 0

    # Retrieve current filter/cut values from the interface.
    current_cuts = {
        'E': energy_entry.get(),
        'A': asymmetry_entry.get(),
        'C': chi2_entry.get(),
        'D': delta_r_entry.get()
    }
    
    # Iterate through each centrality category for plotting.
    for i, (color, marker) in enumerate(zip(colors, markers)):
        # Skip plotting if the centrality category is not visible.
        if not centrality_visibility[labels[i]]:
            continue
            
        # Calculate offsets to avoid point overlapping.
        base_jitter_offset = (i - 1.5) * 0.05
        overlay_jitter_offset = plot_update_count_RelativeSignalError * 0.08
        total_jitter_offset = base_jitter_offset + overlay_jitter_offset
        
        # Determine indices for data points in this category.
        indices = range(i * 3, (i + 1) * 3)
        
        # Calculate x-values and relative signal error values for each point.
        x_vals = [x + total_jitter_offset for x in x_points_base]
        relSig_vals = [filtered_data.iloc[j]['RelativeSignalError'] for j in indices]
        S_B_ratios = [filtered_data.iloc[j]['S/B'] for j in indices]
        
         # Plot each data point.
        for j, x_val in enumerate(x_vals):
            index = i * 3 + j  # Calculate the point index
            relSig_val = relSig_vals[j]
            S_B_ratio = S_B_ratios[j]
            point_id = (plot_update_count_RelativeSignalError, i, j)
            
            # Store point data in the history dictionary.
            plot_RelativeSignalError_history[point_id] = {
                'relSig_val': relSig_val, 'cuts': current_cuts.copy(), 'centrality': labels[i], 'csv_index': index, 'S_B_ratio': S_B_ratio
            }
            
            # Create a line plot for the point and update the line_index_mapping.
            line, = ax.plot(x_val, relSig_val, marker=marker, color=color)
            line.set_gid(point_id)  # Assign group ID to the line.
            line.set_picker(5)  # Enable picking on the plot point.
            line_index_mapping[point_id] = len(ax.get_lines()) - 1  # Map point_id to line index.

        # Update the maximum y-value based on plotted data.
        local_max_y = max(local_max_y, max(relSig_vals))  # Adjusted to not consider error bars

    # Update global maximum y-value for Relative Signal Error.
    if local_max_y > global_max_y_RelativeSignalError:
        global_max_y_RelativeSignalError = local_max_y
        
    # Set y-axis limits with an upper buffer factor for better visualization.
    upper_buffer_factor = 1.25
    ax.set_ylim(0, global_max_y_RelativeSignalError * upper_buffer_factor)
    
    # Set x-axis properties.
    ax.set_xticks(x_ticks)
    ax.set_xlim(1.5, 5.5)
    
    # Add titles and labels to the plot.
    ax.set_title(r"Relative Signal Error")
    ax.set_xlabel(r"$\pi^0$ pT")
    ax.set_ylabel(r"Relative Signal Error")
    
    # Redraw the canvas with the updated plot.
    canvas.draw()

# Define a custom dialog class that inherits from the simpledialog.Dialog class from tkinter.
class InfoDialog(simpledialog.Dialog):

    # Initialize the dialog with various parameters.
    def __init__(self, parent, title, data, index, plot_type):
        self.data = data  # Store the data associated with the clicked point.
        self.index = index  # Store the index of the clicked point.
        self.plot_type = plot_type  # Store the type of plot from which the point comes.
        super().__init__(parent, title)  # Initialize the base class with the parent widget and title.
        
    # Method to create the body of the dialog. It populates the dialog with labels based on the data.
    def body(self, frame):
        # Display various pieces of information as labels within the dialog.
        tk.Label(frame, text=f"Index: {self.data['index']}").pack()  # Show the index of the data point.
        tk.Label(frame, text=f"Cut Values: {self.data['cuts']}").pack()  # Show the cut values used.
        tk.Label(frame, text=f"S/B: {self.data['S_B_ratio']:.4f}").pack()  # Show the signal-to-background ratio.
        
        # Display information specific to the type of data plotted.
        if self.plot_type == "SignalYield":
            # Display yield and related error for Signal Yield.
            tk.Label(frame, text=f"Yield: {self.data['yield']:.4f}").pack()
            tk.Label(frame, text=f"Yield Error: {self.data['yerr']:.4f}").pack()
            tk.Label(frame, text=f"Relative Error: {self.data['rel_err']:.4%}").pack()
        elif self.plot_type == "GaussianMean":
            # Display mean and error for Gaussian Mean.
            tk.Label(frame, text=f"Mean: {self.data['mean']:.7f}").pack()
            tk.Label(frame, text=f"Mean Error: {self.data['yerr']:.7f}").pack()
        elif self.plot_type == "GaussianSigma":
            # Display sigma and error for Gaussian Sigma.
            tk.Label(frame, text=f"Sigma: {self.data['sigma']:.7f}").pack()
            tk.Label(frame, text=f"Sigma Error: {self.data['yerr']:.7f}").pack()
        elif self.plot_type == "RelativeSignalError":
            # Display relative signal error for Relative Signal Error.
            tk.Label(frame, text=f"Rel Sig Err: {self.data['relSigErr']:.7f}").pack()
            
        return frame # Return the frame as the body of the dialog.
        
    # Method to create the button box at the bottom of the dialog.
    def buttonbox(self):
        box = tk.Frame(self) # Create a frame to hold the button.
        w = tk.Button(box, text="OK", width=10, command=self.ok, default=tk.ACTIVE)
        w.pack(side=tk.LEFT, padx=5, pady=5) # Pack the OK button into the frame.
        self.bind("<Return>", self.ok) # Bind the Return key to the OK button action.
        box.pack() # Pack the box into the dialog.
        
# Function to handle click events on the plot.
def on_click(event, root):
    global current_plot_type # Access the global variable to determine the current plot type.
    if hasattr(event.artist, 'get_gid'):
        index = event.artist.get_gid() # Get the group ID (gid) of the clicked artist (point).
        # Get the history dictionary for the current plot type.
        history_dict = get_history_dict_based_on_plot_type(current_plot_type)
        if index in history_dict:
            data_point = history_dict[index] # Get the data for the clicked point.
            data = build_data_for_dialog(data_point, current_plot_type) # Build the data structure for the dialog.
            InfoDialog(root, "Point Information", data, index, current_plot_type) # Create and show the info dialog.
        else:
            print("Data for this point is not found.") # Handle cases where the point data is not found.
            
# Function to build the data structure for the dialog based on the plot type and data point.
def build_data_for_dialog(data_point, plot_type):
    # Build and return a dictionary of data based on the plot type.
    # Includes formatted strings and calculations as needed.
    if plot_type == "SignalYield":
        return {
            'cuts': ", ".join(f"{k}={v}" for k, v in data_point['cuts'].items()),
            'index': data_point.get('csv_index', 'N/A'),
            'yield': data_point['y_val'],
            'yerr': data_point['y_err'],
            'S_B_ratio': data_point['S_B_ratio'],
            'rel_err': (data_point['y_err'] / data_point['y_val']) if data_point['y_val'] != 0 else 0
            
        }
    elif plot_type == "GaussianMean":
        return {
            'cuts': ", ".join(f"{k}={v}" for k, v in data_point['cuts'].items()),
            'index': data_point.get('csv_index', 'N/A'),
            'mean': data_point['mean_val'],
            'yerr': data_point['mean_err'],
            'S_B_ratio': data_point['S_B_ratio'],
        }
    elif plot_type == "GaussianSigma":
        return {
            'cuts': ", ".join(f"{k}={v}" for k, v in data_point['cuts'].items()),
            'index': data_point.get('csv_index', 'N/A'),
            'sigma': data_point['sigma_val'],
            'yerr': data_point['sigma_err'],
            'S_B_ratio': data_point['S_B_ratio'],
        }
    elif plot_type == "RelativeSignalError":
        return {
            'cuts': ", ".join(f"{k}={v}" for k, v in data_point['cuts'].items()),
            'index': data_point.get('csv_index', 'N/A'),
            'relSigErr': data_point['relSig_val'],
            'S_B_ratio': data_point['S_B_ratio'],
        }
    return {}
# Function to return the appropriate history dictionary based on the current plot type.
def get_history_dict_based_on_plot_type(plot_type):
    # Based on the plot_type argument, return the corresponding history dictionary.
    # Each dictionary stores historical data for different types of plots.
    if plot_type == "SignalYield":
        return plot_SignalYield_history
    elif plot_type == "GaussianMean":
        return plot_GaussianMean_history
    elif plot_type == "GaussianSigma":
        return plot_GaussianSigma_history
    elif plot_type == "RelativeSignalError":
        return plot_RelativeSignalError_history

# Function to update the plot based on the current cut values.
def update_plot(clear_plot=True):
    try:
        # Retrieve cut values entered by the user and convert them to float for processing.
        energy = float(energy_entry.get())
        asymmetry = float(asymmetry_entry.get())
        chi2 = float(chi2_entry.get())
        delta_r = float(delta_r_entry.get())

        # Filter the dataset based on the current cut values.
        filtered_data = data[(data['Energy'] == energy) & (data['Asymmetry'] == asymmetry) & (data['Chi2'] == chi2) & (data['DeltaR'] == delta_r)]
        
        # If no data matches the cut values, show an error message and return.
        if filtered_data.empty:
            messagebox.showerror("Error", "No data matches the specified cut values.")
            return

        # Based on the current plot type, call the appropriate plotting function.
        if current_plot_type == "SignalYield":
            plot_SignalYield(filtered_data, clear_plot)
        elif current_plot_type == "GaussianMean":
            plot_GaussianMean(filtered_data, clear_plot)
        elif current_plot_type == "GaussianSigma":
            plot_GaussianSigma(filtered_data, clear_plot)
        elif current_plot_type == "RelativeSignalError":
            plot_RelativeSignalError(filtered_data, clear_plot)
    except ValueError:
        # Catch any ValueError (e.g., from invalid cut value formats) and show an error message.
        messagebox.showerror("Error", "Invalid cut values entered.")

# Function to overlay new cuts on the existing plot without clearing it.
def overlay_new_cuts():
    # Call update_plot with clear_plot set to False, indicating an overlay is required.
    update_plot(clear_plot=False)

# Function to save the current plot to a PDF file.
def save_plot():
    # Open a file dialog to choose the save location and file name.
    file_path = filedialog.asksaveasfilename(defaultextension=".pdf",
                                             filetypes=[("PDF files", "*.pdf")])
    if file_path:
        # If a file path is selected, save the current figure to that path.
        fig.savefig(file_path)

# Function to exit the program.
def exit_program():
    # Terminate the program execution.
    sys.exit()

# Function to create the main application window with all its widgets.
def create_main_application(root):
    # Define global variables that will be used in this function.
    global fig, ax, canvas, energy_entry, asymmetry_entry, chi2_entry, delta_r_entry
    
    # Make the root window visible and set its title.
    root.deiconify()
    root.title("Pi0 Yield Analysis")
    
    # Create a frame in the root window to hold all other widgets.
    frame = ttk.Frame(root)
    frame.pack(fill=tk.BOTH, expand=True)
    
    # Create an entry widget for energy input and position it in the grid layout.
    energy_entry = ttk.Entry(frame)
    energy_entry.grid(row=0, column=1, padx=5, pady=5)
    
    # Place a label next to the energy entry field.
    ttk.Label(frame, text="Energy:").grid(row=0, column=0, padx=5, pady=5)
    
    # Repeat the process for asymmetry, chi2, and delta_r input fields.
    asymmetry_entry = ttk.Entry(frame)
    asymmetry_entry.grid(row=1, column=1, padx=5, pady=5)
    ttk.Label(frame, text="Asymmetry:").grid(row=1, column=0, padx=5, pady=5)
    
    chi2_entry = ttk.Entry(frame)
    chi2_entry.grid(row=2, column=1, padx=5, pady=5)
    ttk.Label(frame, text="Chi2:").grid(row=2, column=0, padx=5, pady=5)
    
    delta_r_entry = ttk.Entry(frame)
    delta_r_entry.grid(row=3, column=1, padx=5, pady=5)
    ttk.Label(frame, text="DeltaR:").grid(row=3, column=0, padx=5, pady=5)
    
    # Create buttons for various actions and place them in the grid.
    update_button = ttk.Button(frame, text="Apply First Cuts/Renew Points", command=lambda: update_plot(clear_plot=True))
    update_button.grid(row=4, column=0, columnspan=2, padx=5, pady=5)
    overlay_button = ttk.Button(frame, text="Overlay New Cuts", command=overlay_new_cuts)
    overlay_button.grid(row=5, column=0, columnspan=2, padx=5, pady=5)
    save_button = ttk.Button(frame, text="Save Plot as PDF", command=save_plot)
    save_button.grid(row=6, column=0, columnspan=2, padx=5, pady=5)
    exit_button = ttk.Button(frame, text="Exit Program", command=exit_program)
    exit_button.grid(row=7, column=0, columnspan=2, padx=5, pady=5)
    
    # Initialize the Matplotlib figure and axis.
    fig, ax = plt.subplots()
    
    # Embed the Matplotlib figure in the Tkinter frame.
    canvas = FigureCanvasTkAgg(fig, master=frame)
    canvas.get_tk_widget().grid(row=8, column=0, columnspan=2, padx=5, pady=5)
    
    # Annotation setup for interactive elements on the plot.
    annot = ax.annotate('', xy=(0, 0), xytext=(20, 20), textcoords="offset points",
                        bbox=dict(boxstyle="round", fc="w"), arrowprops=dict(arrowstyle="->"))
    annot.set_visible(False)
    
    # Connect a function to handle click events on the plot.
    fig.canvas.mpl_connect('pick_event', lambda event: on_click(event, root))
    
    # Frame for centrality toggle buttons.
    centrality_frame = ttk.LabelFrame(frame, text="Hide Centrality:")
    centrality_frame.grid(row=9, column=0, columnspan=2, padx=5, pady=5, sticky="ew")
    
    # Modify the command to pass 'root' to the 'create_analysis_window' function
    analyze_button = ttk.Button(frame, text="Analyze Data on Screen", command=lambda: create_analysis_window(root))
    analyze_button.grid(row=10, column=0, columnspan=2, padx=5, pady=5)


    
    # Function to toggle visibility of plot elements based on centrality.
    def toggle_centrality(label):
        global labels, centrality_visibility, errorbar_containers_SignalYield, errorbar_containers_GaussianMean, line_index_mapping
        centrality_visibility[label] = not centrality_visibility[label]
        
        # Loop through containers of different plot types to toggle visibility.
        for container in errorbar_containers_SignalYield:
            if container[0].get_gid():
                point_id = container[0].get_gid()
                centrality = plot_SignalYield_history.get(point_id, {}).get('centrality')
                if centrality == label:
                    toggle_visibility(container, centrality_visibility[label])
        for container in errorbar_containers_GaussianMean:
            if container[0].get_gid():
                point_id = container[0].get_gid()
                centrality = plot_GaussianMean_history.get(point_id, {}).get('centrality')
                if centrality == label:
                    toggle_visibility(container, centrality_visibility[label])
        canvas.draw_idle()
        for container in errorbar_containers_GaussianSigma:
            if container[0].get_gid():
                point_id = container[0].get_gid()
                centrality = plot_GaussianSigma_history.get(point_id, {}).get('centrality')
                if centrality == label:
                    toggle_visibility(container, centrality_visibility[label])
        canvas.draw_idle()
        
        # Special handling for Relative Signal Error plot.
        for point_id, point_data in plot_RelativeSignalError_history.items():
            if point_data.get('centrality') == label:
                line_index = line_index_mapping.get(point_id)
                if line_index is not None:
                    line = ax.get_lines()[line_index]
                    line.set_visible(centrality_visibility[label])

        canvas.draw_idle()
        
    # Function to set the visibility of plot elements in a container.
    def toggle_visibility(container, visibility):
        container[0].set_visible(visibility)
        for capline in container[2]:
            capline.set_visible(visibility)
        for barline_cols in container[1]:
            for barline in barline_cols:
                barline.set_visible(visibility)
                
     # Creating toggle buttons for each centrality category.
    for label in ["60-100%", "40-60%", "20-40%", "0-20%"]:
        check_var = tk.BooleanVar(value=True)  # Initialize variable to True
        check_button = ttk.Checkbutton(centrality_frame, text=label, variable=check_var,
                                       command=lambda l=label: toggle_centrality(l))
        check_button.var = check_var  # Store the variable within the button for easy access
        check_button.pack(side=tk.LEFT, padx=10)
        
    # Frame for the navigation toolbar of Matplotlib.
    toolbar_frame = ttk.Frame(root)
    toolbar_frame.pack(fill=tk.X, expand=True)
    
    # Add the navigation toolbar for the plot.
    toolbar = NavigationToolbar2Tk(canvas, toolbar_frame)
    toolbar.update()
    
    # Final draw call for the canvas.
    canvas.draw()
    
    
def create_initial_window(root):
    initial_window = tk.Toplevel(root)
    initial_window.title("What data would you like to analyze?")
    def set_plot_type_and_continue(plot_type):
        global current_plot_type
        current_plot_type = plot_type
        initial_window.destroy()
        create_main_application(root)
    ttk.Button(initial_window, text="Signal Yield", command=lambda: set_plot_type_and_continue("SignalYield")).pack()
    ttk.Button(initial_window, text="Relative Signal Error", command=lambda: set_plot_type_and_continue("RelativeSignalError")).pack()
    ttk.Button(initial_window, text="Gaussian Mean", command=lambda: set_plot_type_and_continue("GaussianMean")).pack()
    ttk.Button(initial_window, text="Gaussian Sigma", command=lambda: set_plot_type_and_continue("GaussianSigma")).pack()
    ttk.Button(initial_window, text="Analyze All CSV Data", command=lambda: create_csv_analysis_window(root, data)).pack()


def create_csv_analysis_window(root, csv_data):
    csv_analysis_window = tk.Toplevel(root)
    csv_analysis_window.title("CSV Data Analysis")

    # Button for sorting the entire CSV dataset by Signal Yield
    ttk.Button(csv_analysis_window, text="Sort Entire Dataset by Signal Yield", command=lambda: sort_csv_by_signal_yield(root, csv_data)).pack()

    # ... [add more buttons for other analysis types as needed] ..

    
def sort_csv_by_signal_yield(root, csv_data):
    # Read the data from the CSV file
    sorted_data = csv_data.sort_values(by='Yield', ascending=False)

    # Define centrality categories based on the 'Index' column
    centrality_categories = {
        '60-100%': [0, 1, 2],
        '40-60%': [3, 4, 5],
        '20-40%': [6, 7, 8],
        '0-20%': [9, 10, 11]
    }
    
    # Create a new window to display sorted results
    results_window = tk.Toplevel(root)
    results_window.title("Sorted Signal Yields from CSV")

    # Create a text widget to display the sorted results
    results_display = tk.Text(results_window)
    results_display.pack()

    # Define tags for styling
    results_display.tag_configure('centrality', foreground='red')
    results_display.tag_configure('index', font=('Helvetica', '10', 'bold'))


    # Build up the entire text string in memory first
    full_text = ""
    for centrality, indices in centrality_categories.items():
        centrality_data = sorted_data[sorted_data['Index'].isin(indices)]
        centrality_data_sorted = centrality_data.sort_values(by='Yield', ascending=False)
        
        full_text += f"{centrality}:\n"
        last_index = -1
        for _, row in centrality_data_sorted.iterrows():
            if last_index != -1 and last_index != row['Index']:
                full_text += "\n"
            index_str = f"Index {int(row['Index'])}: Yield: {row['Yield']}, "
            cuts_str = f"Cuts: E: {row['Energy']}, A: {row['Asymmetry']}, C: {row['Chi2']}, D: {row['DeltaR']}\n"
            full_text += index_str + cuts_str
            last_index = row['Index']
        full_text += "\n"

    # Now, insert the entire text at once
    results_display.insert(tk.END, full_text)
    

    # Scroll back to the top of the text widget
    results_display.see('1.0')

    
    
# Function to create the side analysis window
def create_analysis_window(root):
    analysis_window = tk.Toplevel(root)
    analysis_window.title("Further Quantitative Analysis")


    if current_plot_type == "SignalYield":
        # Button for sorting Signal Yield data
        ttk.Button(analysis_window, text="Sort by Signal Yield", command=lambda: sort_by_signal_yield(root)).pack()
        ttk.Button(analysis_window, text="Sort by Highest Yield and S/B", command=lambda: sort_by_yield_and_sb(root)).pack()
    elif current_plot_type == "GaussianMean":
        # Button for sorting Gaussian Mean data
        ttk.Button(analysis_window, text="Sort by Distance from π⁰ mass", command=lambda: sort_by_gaussian_mean(root)).pack()


    # ... Add more buttons for other analysis features here ...


def sort_csv_by_signal_yield(root, csv_data):
    # Read the data from the CSV file
    sorted_data = csv_data.sort_values(by='Yield', ascending=False)

    # Define centrality categories based on the 'Index' column
    centrality_categories = {
        '60-100%': [0, 1, 2],
        '40-60%': [3, 4, 5],
        '20-40%': [6, 7, 8],
        '0-20%': [9, 10, 11]
    }

    # Create a new window to display sorted results
    results_window = tk.Toplevel(root)
    results_window.title("Sorted Signal Yields from CSV")

    # Create a text widget to display the sorted results
    results_display = tk.Text(results_window)
    results_display.pack()

    # Define tags for styling
    results_display.tag_configure('centrality', foreground='red', font=('Helvetica', '10', 'bold'))
    results_display.tag_configure('index', font=('Helvetica', '10', 'bold'))

    # Insert data and apply tags directly to avoid incorrect tag application
    for centrality, indices in centrality_categories.items():
        # Insert centrality label with the 'centrality' tag
        results_display.insert(tk.END, f"{centrality}:\n", 'centrality')

        centrality_data = sorted_data[sorted_data['Index'].isin(indices)]
        centrality_data_sorted = centrality_data.sort_values(by=['Index', 'Yield'], ascending=[True, False])

        last_index = -1
        for _, row in centrality_data_sorted.iterrows():
            if last_index != row['Index']:
                if last_index != -1:
                    # Ensure there is a newline between groups of different indices
                    results_display.insert(tk.END, "\n")
                last_index = row['Index']

            # Construct strings for each line
            index_label = f"Index {int(row['Index'])}: "
            yield_str = f"Yield: {row['Yield']}, "
            cuts_str = f"Cuts: E: {row['Energy']}, A: {row['Asymmetry']}, C: {row['Chi2']}, D: {row['DeltaR']}\n"

            # Insert index label with 'index' tag for bold styling
            results_display.insert(tk.END, index_label, 'index')
            # Insert the rest of the line without any tag for normal styling
            results_display.insert(tk.END, yield_str + cuts_str)

        # Ensure there is a newline after each centrality section
        results_display.insert(tk.END, "\n")

    # Scroll back to the top of the text widget
    results_display.see('1.0')


# Function to sort signal yield data by both yield and S/B ratio
def sort_by_yield_and_sb(root):
    # Filter out the data points that are not currently visible
    visible_data = {point_id: data for point_id, data in plot_SignalYield_history.items()
                    if centrality_visibility[data['centrality']]}

    # Group data by centrality and then sort by yield and S/B within each group
    grouped_data = {}
    for data in visible_data.values():
        centrality_group = data['centrality']
        if centrality_group not in grouped_data:
            grouped_data[centrality_group] = []
        grouped_data[centrality_group].append(data)

    # Sort each group first by yield and then by S/B ratio
    # This sorting logic means that the yield is always the primary factor in determining the order.
    # The S/B ratio is only considered when there is a tie in yield values.
    for centrality in grouped_data:
        grouped_data[centrality].sort(key=lambda x: (-x['y_val'], -x['S_B_ratio']))

    # Create a new window to display sorted results
    results_window = tk.Toplevel(root)
    results_window.title("Sorted Signal Yields by Yield and S/B")

    # Create a text widget to display the sorted results
    results_display = tk.Text(results_window)
    results_display.pack()

    # Define tags for styling
    results_display.tag_configure('centrality', foreground='red')
    results_display.tag_configure('index', font=('Helvetica', '10', 'bold'))

    # Insert sorted data into the text widget, grouped by centrality
    for centrality, data_list in grouped_data.items():
        results_display.insert(tk.END, f"{centrality}:\n", 'centrality')
        last_index = None
        for data in data_list:
            if last_index is not None and last_index != data['csv_index']:
                results_display.insert(tk.END, "\n")  # Add a newline for new index groups
            cuts_str = ", ".join(f"{k}: {v}" for k, v in data['cuts'].items())
            results_display.insert(tk.END, f"Index {data['csv_index']}: ", 'index')
            results_display.insert(tk.END, f"Yield: {data['y_val']}, S/B: {data['S_B_ratio']}, Cuts: {cuts_str}\n")
            last_index = data['csv_index']
        results_display.insert(tk.END, "\n\n")

    # Scroll back to the top of the text widget
    results_display.see('1.0')

    

# Function to perform the sorting of Gaussian Mean data and display results
def sort_by_gaussian_mean(root):
    # Filter out the data points that are not currently visible
    visible_data = {point_id: data for point_id, data in plot_GaussianMean_history.items()
                    if centrality_visibility[data['centrality']]}

    # Group data by centrality and then by index within each group
    grouped_data = {}
    pi0_mass = 0.135  # Theoretical π⁰ mass
    for data in visible_data.values():
        centrality_group = data['centrality']
        if centrality_group not in grouped_data:
            grouped_data[centrality_group] = {}
        index_group = data['csv_index']
        if index_group not in grouped_data[centrality_group]:
            grouped_data[centrality_group][index_group] = []
        grouped_data[centrality_group][index_group].append(data)

    # Sort each index group by the absolute difference from π⁰ mass
    for centrality in grouped_data:
        for index in grouped_data[centrality]:
            grouped_data[centrality][index].sort(key=lambda x: abs(x['mean_val'] - pi0_mass))

    # Create a new window to display sorted results
    results_window = tk.Toplevel(root)
    results_window.title("Sorted Gaussian Mean Data")

    # Create a text widget or a listbox to display the sorted results
    results_display = tk.Text(results_window)
    results_display.pack()

    # Define tags for styling
    results_display.tag_configure('centrality', foreground='red')
    results_display.tag_configure('index', font=('Helvetica', '10', 'bold'))

    # Insert sorted data into the text widget, grouped by centrality and then by index
    for centrality, index_groups in grouped_data.items():
        centrality_position = results_display.index(tk.END)
        results_display.insert(tk.END, f"{centrality}:\n", 'centrality')
        for index, data_list in sorted(index_groups.items()):
            last_index = None
            for data in data_list:
                if last_index is not None and last_index != data['csv_index']:
                    results_display.insert(tk.END, "\n")  # Add a newline for new index groups
                index_position = results_display.index(tk.END)
                cuts_str = ", ".join(f"{k}: {v}" for k, v in data['cuts'].items())
                results_display.insert(tk.END, f"Index {data['csv_index']}: ", 'index')
                results_display.insert(tk.END, f"Mean: {data['mean_val']}, Cuts: {cuts_str}\n")
                last_index = data['csv_index']
            results_display.insert(tk.END, "\n")  # Add a newline after each index group
        results_display.insert(tk.END, "\n\n")  # Add extra newline after each centrality group

    # Scroll back to the top of the text widget
    results_display.see('1.0')



def main():
    global fig, ax, canvas
    root = tk.Tk()
    root.withdraw()
    create_initial_window(root)
    root.mainloop()
if __name__ == "__main__":
    main()
