# Import necessary libraries
import tkinter as tk  # Tkinter for GUI elements
from tkinter import ttk, messagebox, filedialog, simpledialog  # Additional Tkinter widgets and dialogs
import pandas as pd  # Pandas for data handling
import matplotlib  # Matplotlib for plotting
matplotlib.use('TkAgg')  # Set Matplotlib to work with Tkinter
import matplotlib.pyplot as plt  # Import pyplot for plotting
from matplotlib.lines import Line2D  # Import Line2D for custom lines
from matplotlib.container import ErrorbarContainer  # For handling error bars
from matplotlib.legend_handler import HandlerErrorbar  # Custom legend handler for error bars
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2Tk  # Tkinter backends for Matplotlib
import sys  # System-specific parameters and functions

# Read data from a CSV file for plotting
data = pd.read_csv('/Users/patsfan753/Desktop/Desktop/AnalyzePi0s_Final/dataOutput/PlotByPlotOutput.csv')

# Initialize global variables to store maximum values for different plot types
global_max_y_signalYield = 0
global_max_y_GaussianMean = 0
global_min_y_GaussianMean = 0
global_max_y_GaussianSigma = 0
global_max_y_RelativeSignalError = 0

# Initialize counters for total points plotted in each type
total_point_count_SignalYield = 0
total_point_count_GaussianMean = 0
total_point_count_GaussianSigma = 0
total_point_count_RelativeSignalError = 0

# Lists to store error bar containers for different plot types
errorbar_containers_SignalYield = []
errorbar_containers_GaussianMean = []
errorbar_containers_GaussianSigma = []

# Dictionaries to store historical data for each plot type
plot_SignalYield_history = {}
plot_GaussianMean_history = {}
plot_GaussianSigma_history = {}
plot_RelativeSignalError_history = {}

# Counters to track the number of updates made to each plot type
plot_update_count_SignalYield = 0
plot_update_count_GaussianMean = 0
plot_update_count_GaussianSigma = 0
plot_update_count_RelativeSignalError = 0

# Define a list of colors for plotting
colors = ['red', 'blue', 'lightgreen', 'deeppink']

# Store the current plot type globally
current_plot_type = "SignalYield"

# Define function to initialize plot legend
def init_legend():
    # Define labels and corresponding colors and markers
    labels = ["60-100%", "40-60%", "20-40%", "0-20%"]
    colors = ['red', 'blue', 'lightgreen', 'deeppink']
    markers = ['o', 's', '^', 'v']
    
    # Create dummy lines to represent legend entries
    dummy_lines = []
    for color, marker, label in zip(colors, markers, labels):
        line = ax.errorbar([], [], yerr=[], fmt=marker, color=color, label=label)
        dummy_lines.append(line)
    
    # Create a handler map for custom handling of error bars in the legend
    handler_map = {ErrorbarContainer: HandlerErrorbar(yerr_size=0.5)}
    
    # Adjust the position of the legend based on the current plot type
    if current_plot_type == "RelativeSignalError":
        legend_loc = 'upper left'  # Legend in the upper left for RelativeSignalError
    else:
        legend_loc = 'upper right'  # Default to upper right for other plot types
    
    # Add the legend to the axis with the defined location and handler map
    ax.legend(handles=dummy_lines, title='Centrality:', handler_map=handler_map, loc=legend_loc)


# Function to plot the Signal Yield.
def plot_SignalYield(filtered_data, clear_plot):
    # Access global variables to update them within the function
    global global_max_y_signalYield, plot_SignalYield_history, plot_update_count_SignalYield, errorbar_containers_SignalYield, total_point_count_SignalYield

    # Clear the plot and reset variables if clear_plot is True
    if clear_plot:
        ax.clear()  # Clear the current axes
        ax.set_yscale('log')  # Set y-axis to logarithmic scale
        global_max_y_signalYield = 0  # Reset the maximum y-value for signal yield
        init_legend()  # Initialize the legend
        errorbar_containers_SignalYield.clear()  # Clear the errorbar containers
        plot_SignalYield_history = {}  # Reset the history of plotted data
        plot_update_count_SignalYield = 0  # Reset the count of plot updates
    else:
        # Increment plot update count if not clearing the plot
        plot_update_count_SignalYield += 1
    
    # Base x-axis points and ticks setup
    x_points_base = [2.5, 3.5, 4.5]  # Base x-values for plotting
    x_ticks = [2, 3, 4, 5]  # Tick marks for the x-axis
    markers = ['o', 's', '^', 'v']  # Different markers for each data series

    # Initialize local maximum y-value for this specific plot update
    local_max_y = 0

    # Retrieve current cuts from the GUI entries
    current_cuts = {
        'E': energy_entry.get(),
        'A': asymmetry_entry.get(),
        'C': chi2_entry.get(),
        'D': delta_r_entry.get()
    }

    # Loop through each data series to plot
    for i, (color, marker) in enumerate(zip(colors, markers)):
        # Calculate jitter offset for x-values to prevent overlapping
        base_jitter_offset = (i - 1.5) * 0.05
        overlay_jitter_offset = plot_update_count_SignalYield * 0.08
        total_jitter_offset = base_jitter_offset + overlay_jitter_offset

        # Determine indices for slicing the filtered data
        indices = range(i * 3, (i + 1) * 3)

        # Adjust x-values and retrieve y-values and errors from the filtered data
        x_vals = [x + total_jitter_offset for x in x_points_base]
        y_vals = [filtered_data.iloc[j]['Yield'] for j in indices]
        y_errs = [filtered_data.iloc[j]['YieldError'] for j in indices]

        # Plot each point with error bars
        for j, (x_val, y_val, y_err) in enumerate(zip(x_vals, y_vals, y_errs)):
            # Calculate a unique point index
            index = i * 3 + j
            point_id = (plot_update_count_SignalYield, i, j)

            # Store point data in history
            plot_SignalYield_history[point_id] = {
                'y_val': y_val, 'y_err': y_err, 'cuts': current_cuts.copy()
            }

            # Plot the point with error bars
            container = ax.errorbar(x_val, y_val, yerr=y_err, fmt=marker, color=color)
            container[0].set_gid(point_id)  # Assign a group ID to the point
            container[0].set_picker(5)  # Enable picking for the point
            errorbar_containers_SignalYield.append(container)  # Store the container

            # Increment the total point count
            total_point_count_SignalYield += 1
            
        # Update the local maximum y-value considering the error bars
        local_max_y = max(local_max_y, max(y_vals) * 10)

    # Update the global maximum y-value if the local max is greater
    if local_max_y > global_max_y_signalYield:
        global_max_y_signalYield = local_max_y

    # Set the limits, ticks, and labels for the plot
    ax.set_ylim(0.1, global_max_y_signalYield)
    ax.set_xticks(x_ticks)
    ax.set_xlim(1.5, 5.5)
    ax.set_title(r"$\pi^0$ Yield")  # Set the title of the plot
    ax.set_xlabel(r"$\pi^0$ pT")  # Set the x-axis label
    ax.set_ylabel(r"$\pi^0$ Signal Yield")  # Set the y-axis label

    # Render the plot onto the canvas
    canvas.draw()

    
# Function to plot Gaussian Mean values.
def plot_GaussianMean(filtered_data, clear_plot):
    # Access and update global variables
    global global_max_y_GaussianMean, global_min_y_GaussianMean, plot_GaussianMean_history, plot_update_count_GaussianMean, errorbar_containers_GaussianMean, total_point_count_GaussianMean
    
    # Clear existing plot and reset variables if needed
    if clear_plot:
        ax.clear()  # Clear the axes for a new plot
        global_max_y_GaussianMean = 0  # Reset the maximum y-value for Gaussian Mean
        global_min_y_GaussianMean = 0  # Reset the minimum y-value for Gaussian Mean
        init_legend()  # Initialize the legend for the plot
        errorbar_containers_GaussianMean.clear()  # Clear any existing errorbar containers
        plot_GaussianMean_history = {}  # Reset the history of plotted Gaussian Mean data
        plot_update_count_GaussianMean = 0  # Reset the count of plot updates
    else:
        plot_update_count_GaussianMean += 1  # Increment the plot update count for subsequent plots
        
    # Base x-points and tick marks for the plot
    x_points_base = [2.5, 3.5, 4.5]  # Base x-values for plotting
    x_ticks = [2, 3, 4, 5]  # Tick marks for the x-axis
    markers = ['o', 's', '^', 'v']  # Different markers for each data series
    
    # Initialize local max and min y-values for this specific plot
    local_max_y = 0
    local_min_y = 0

    # Retrieve current cuts (filters) from the GUI entries
    current_cuts = {
        'E': energy_entry.get(),
        'A': asymmetry_entry.get(),
        'C': chi2_entry.get(),
        'D': delta_r_entry.get()
    }

    # Loop through each data series for plotting
    for i, (color, marker) in enumerate(zip(colors, markers)):
        # Calculate jitter offset for x-values to prevent overlapping
        base_jitter_offset = (i - 1.5) * 0.05
        overlay_jitter_offset = plot_update_count_GaussianMean * 0.08
        total_jitter_offset = base_jitter_offset + overlay_jitter_offset

        # Determine indices for slicing the filtered data
        indices = range(i * 3, (i + 1) * 3)

        # Adjust x-values and retrieve Gaussian Mean values and errors
        x_vals = [x + total_jitter_offset for x in x_points_base]
        mean_vals = [filtered_data.iloc[j]['GaussMean'] for j in indices]
        mean_errs = [filtered_data.iloc[j]['GaussMeanError'] for j in indices]

        # Plot each point with error bars
        for j, (x_val, mean_val, mean_err) in enumerate(zip(x_vals, mean_vals, mean_errs)):
            # Calculate a unique point index for identification
            index = i * 3 + j
            point_id = (plot_update_count_GaussianMean, i, j)

            # Store point data in history for future reference
            plot_GaussianMean_history[point_id] = {
                'mean_val': mean_val, 'mean_err': mean_err, 'cuts': current_cuts.copy()
            }

            # Plot the point with error bars
            container = ax.errorbar(x_val, mean_val, yerr=mean_err, fmt=marker, color=color)
            container[0].set_gid(point_id)  # Assign a group ID to the point
            container[0].set_picker(5)  # Enable picking for the point
            errorbar_containers_GaussianMean.append(container)  # Store the container

            # Increment the total point count
            total_point_count_GaussianMean += 1
            
        # Update local max and min y-values including error bars
        max_y_val_with_err = max(mean_val + mean_err for mean_val, mean_err in zip(mean_vals, mean_errs))
        min_y_val_with_err = min(mean_val - mean_err for mean_val, mean_err in zip(mean_vals, mean_errs))
        local_max_y = max(local_max_y, max_y_val_with_err)
        local_min_y = min(local_min_y, min_y_val_with_err)

    # Update the global max and min y-values if necessary
    if local_max_y > global_max_y_GaussianMean:
        global_max_y_GaussianMean = local_max_y
    if local_min_y < global_min_y_GaussianMean:
        global_min_y_GaussianMean = local_min_y

    # Set buffer factors for the y-axis limits
    upper_buffer_factor = 1.5  # 50% extra space above the highest point
    lower_buffer_factor = 0.1  # 10% buffer below the lowest point

    # Set the y-axis limits with added buffers
    ax.set_ylim(global_min_y_GaussianMean - (global_min_y_GaussianMean * lower_buffer_factor),
                global_max_y_GaussianMean * upper_buffer_factor)

    # Draw a horizontal line indicating pi0 mass
    pi0_mass = 0.135  # pi0 mass value
    ax.axhline(y=pi0_mass, color='gray', linestyle='--')  # Horizontal line for pi0 mass
    ax.text(1.6, pi0_mass, r'$\pi^0$ mass', va='center', ha='left', backgroundcolor='white')  # Text label for the pi0 mass line

    # Set the x-axis ticks and limits
    ax.set_xticks(x_ticks)
    ax.set_xlim(1.5, 5.5)

    # Set titles and labels for the plot
    ax.set_title(r"Gaussian Mean Values vs $\pi^0$ pT")
    ax.set_xlabel(r"$\pi^0$ pT")
    ax.set_ylabel(r"Gaussian Mean")

    # Render the plot onto the canvas
    canvas.draw()


# Function to plot Gaussian Sigma values.
def plot_GaussianSigma(filtered_data, clear_plot):
    # Access and update global variables
    global global_max_y_GaussianSigma, global_min_y_GaussianSigma, plot_GaussianSigma_history, plot_update_count_GaussianSigma, errorbar_containers_GaussianSigma, total_point_count_GaussianSigma
    
    # Clear existing plot and reset variables if needed
    if clear_plot:
        ax.clear()  # Clear the axes for a new plot
        global_max_y_GaussianSigma = 0  # Reset the maximum y-value for Gaussian Sigma
        global_min_y_GaussianSigma = 0  # Reset the minimum y-value for Gaussian Sigma
        init_legend()  # Initialize the legend for the plot
        errorbar_containers_GaussianSigma.clear()  # Clear any existing errorbar containers
        plot_GaussianSigma_history = {}  # Reset the history of plotted Gaussian Sigma data
        plot_update_count_GaussianSigma = 0  # Reset the count of plot updates
    else:
        plot_update_count_GaussianSigma += 1  # Increment the plot update count for subsequent plots

    # Base x-points and tick marks for the plot
    x_points_base = [2.5, 3.5, 4.5]  # Base x-values for plotting
    x_ticks = [2, 3, 4, 5]  # Tick marks for the x-axis
    markers = ['o', 's', '^', 'v']  # Different markers for each data series

    # Initialize local max and min y-values for this specific plot
    local_max_y = 0
    local_min_y = 0

    # Retrieve current cuts (filters) from the GUI entries
    current_cuts = {
        'E': energy_entry.get(),
        'A': asymmetry_entry.get(),
        'C': chi2_entry.get(),
        'D': delta_r_entry.get()
    }

    # Loop through each data series for plotting
    for i, (color, marker) in enumerate(zip(colors, markers)):
        # Calculate jitter offset for x-values to prevent overlapping
        base_jitter_offset = (i - 1.5) * 0.05
        overlay_jitter_offset = plot_update_count_GaussianSigma * 0.08
        total_jitter_offset = base_jitter_offset + overlay_jitter_offset

        # Determine indices for slicing the filtered data
        indices = range(i * 3, (i + 1) * 3)

        # Adjust x-values and retrieve Gaussian Sigma values and errors
        x_vals = [x + total_jitter_offset for x in x_points_base]
        sigma_vals = [filtered_data.iloc[j]['GaussSigma'] for j in indices]
        sigma_errs = [filtered_data.iloc[j]['GaussSigmaError'] for j in indices]

        # Plot each point with error bars
        for j, (x_val, sigma_val, sigma_err) in enumerate(zip(x_vals, sigma_vals, sigma_errs)):
            # Calculate a unique point index for identification
            index = i * 3 + j
            point_id = (plot_update_count_GaussianSigma, i, j)

            # Store point data in history for future reference
            plot_GaussianSigma_history[point_id] = {
                'sigma_val': sigma_val, 'sigma_err': sigma_err, 'cuts': current_cuts.copy()
            }

            # Plot the point with error bars
            container = ax.errorbar(x_val, sigma_val, yerr=sigma_err, fmt=marker, color=color)
            container[0].set_gid(point_id)  # Assign a group ID to the point
            container[0].set_picker(5)  # Enable picking for the point
            errorbar_containers_GaussianSigma.append(container)  # Store the container

            # Increment the total point count
            total_point_count_GaussianSigma += 1

        # Update local max and min y-values including error bars
        max_y_val_with_err = max(sigma_val + sigma_err for sigma_val, sigma_err in zip(sigma_vals, sigma_errs))
        min_y_val_with_err = min(sigma_val - sigma_err for sigma_val, sigma_err in zip(sigma_vals, sigma_errs))
        local_max_y = max(local_max_y, max_y_val_with_err)
        local_min_y = min(local_min_y, min_y_val_with_err)

    # Update the global max and min y-values if necessary
    if local_max_y > global_max_y_GaussianSigma:
        global_max_y_GaussianSigma = local_max_y
    if local_min_y < global_min_y_GaussianSigma:
        global_min_y_GaussianSigma = local_min_y
    else:
        global_min_y_GaussianSigma = 0  # Default lower bound
        
    # Set the y-axis limit to provide extra space above the highest point
    upper_buffer_factor = 1.5
    lower_buffer_factor = 0.1
    # Set the y-axis limits with a buffer
    ax.set_ylim(global_min_y_GaussianSigma - (global_min_y_GaussianSigma * lower_buffer_factor),
            global_max_y_GaussianSigma * upper_buffer_factor)

    ax.set_xticks(x_ticks)
    ax.set_xlim(1.5, 5.5)
    ax.set_title(r"Gaussian Sigma Values vs $\pi^0$ pT")
    ax.set_xlabel(r"$\pi^0$ pT")
    ax.set_ylabel(r"Gaussian Sigma")
    canvas.draw()


# Function to plot Relative Signal Error values.
def plot_RelativeSignalError(filtered_data, clear_plot):
    global global_max_y_RelativeSignalError, plot_RelativeSignalError_history, plot_update_count_RelativeSignalError, total_point_count_RelativeSignalError
    if clear_plot:
        ax.clear()
        global_max_y_RelativeSignalError = 0
        init_legend()
        plot_RelativeSignalError_history = {}
        plot_update_count_RelativeSignalError = 0
    else:
        plot_update_count_RelativeSignalError += 1
    x_points_base = [2.5, 3.5, 4.5]
    x_ticks = [2, 3, 4, 5]
    markers = ['o', 's', '^', 'v']
    local_max_y = 0
    current_cuts = {
        'E': energy_entry.get(),
        'A': asymmetry_entry.get(),
        'C': chi2_entry.get(),
        'D': delta_r_entry.get()
    }
    for i, (color, marker) in enumerate(zip(colors, markers)):
        base_jitter_offset = (i - 1.5) * 0.05
        overlay_jitter_offset = plot_update_count_RelativeSignalError * 0.08
        total_jitter_offset = base_jitter_offset + overlay_jitter_offset
        indices = range(i * 3, (i + 1) * 3)
        x_vals = [x + total_jitter_offset for x in x_points_base]
        relSig_vals = [filtered_data.iloc[j]['RelativeSignalError'] for j in indices]
        for j, x_val in enumerate(x_vals):
            relSig_val = relSig_vals[j]
            point_id = (plot_update_count_RelativeSignalError, i, j)
            plot_RelativeSignalError_history[point_id] = {
                'relSig_val': relSig_val, 'cuts': current_cuts.copy()
            }

            # Plot each point and make it interactive
            line, = ax.plot(x_val, relSig_val, marker=marker, color=color)
            line.set_gid(point_id)
            line.set_picker(5)

            
            
        local_max_y = max(local_max_y, max(relSig_vals))  # Adjusted to not consider error bars
    if local_max_y > global_max_y_RelativeSignalError:
        global_max_y_RelativeSignalError = local_max_y
    upper_buffer_factor = 1.25
    ax.set_ylim(0, global_max_y_RelativeSignalError * upper_buffer_factor)
    ax.set_xticks(x_ticks)
    ax.set_xlim(1.5, 5.5)
    ax.set_title(r"Relative Signal Error")
    ax.set_xlabel(r"$\pi^0$ pT")
    ax.set_ylabel(r"Relative Signal Error")
    canvas.draw()



# Class to create an information dialog window
class InfoDialog(simpledialog.Dialog):
    # Constructor for the InfoDialog class
    def __init__(self, parent, title, data, index, plot_type):
        self.data = data  # Data to display
        self.index = index  # Index of the data point
        self.plot_type = plot_type  # Type of plot being displayed
        super().__init__(parent, title)  # Initialize the superclass

    # Method to build the dialog body
    def body(self, frame):
        # Display cut values
        tk.Label(frame, text=f"Cut Values: {self.data['cuts']}").pack()
        # Display information based on plot type
        if self.plot_type == "SignalYield":
            # For Signal Yield plot type
            tk.Label(frame, text=f"Yield: {self.data['yield']:.2f}").pack()
            tk.Label(frame, text=f"Yield Error: {self.data['yerr']:.2f}").pack()
            tk.Label(frame, text=f"Relative Error: {self.data['rel_err']:.2%}").pack()
        elif self.plot_type == "GaussianMean":
            # For Gaussian Mean plot type
            tk.Label(frame, text=f"Mean: {self.data['mean']:.7f}").pack()
            tk.Label(frame, text=f"Mean Error: {self.data['yerr']:.7f}").pack()
        elif self.plot_type == "GaussianSigma":
            # For Gaussian Sigma plot type
            tk.Label(frame, text=f"Sigma: {self.data['sigma']:.7f}").pack()
            tk.Label(frame, text=f"Sigma Error: {self.data['yerr']:.7f}").pack()
        elif self.plot_type == "RelativeSignalError":
            # For Relative Signal Error plot type
            tk.Label(frame, text=f"Rel Sig Err: {self.data['relSigErr']:.5f}").pack()
        return frame  # Return the frame as the body of the dialog

    # Method to create the dialog's button box
    def buttonbox(self):
        box = tk.Frame(self)  # Create a frame for buttons
        w = tk.Button(box, text="OK", width=10, command=self.ok, default=tk.ACTIVE)
        w.pack(side=tk.LEFT, padx=5, pady=5)  # Add OK button
        self.bind("<Return>", self.ok)  # Bind Return key to OK action
        box.pack()  # Pack the box into the dialog

# Function to handle click events on the plot
def on_click(event, root):
    global current_plot_type  # Access global variable for current plot type
    if hasattr(event.artist, 'get_gid'):
        index = event.artist.get_gid()  # Get the unique identifier for the clicked point
        history_dict = get_history_dict_based_on_plot_type(current_plot_type)  # Get the history dictionary for the current plot type

        if index in history_dict:
            data_point = history_dict[index]  # Retrieve the data for the clicked point
            data = build_data_for_dialog(data_point, current_plot_type)  # Build data dictionary for the dialog
            InfoDialog(root, "Point Information", data, index, current_plot_type)  # Display the info dialog
        else:
            print("Data for this point is not found.")  # Print message if data not found

# Function to build data dictionary for info dialog
def build_data_for_dialog(data_point, plot_type):
    # Construct and return the data dictionary based on plot type
    if plot_type == "SignalYield":
        return {
            'cuts': ", ".join(f"{k}={v}" for k, v in data_point['cuts'].items()),
            'yield': data_point['y_val'],
            'yerr': data_point['y_err'],
            'rel_err': (data_point['y_err'] / data_point['y_val']) if data_point['y_val'] != 0 else 0
        }
    # Additional conditions for other plot types (GaussianMean, GaussianSigma, RelativeSignalError) 
    elif plot_type == "GaussianMean":
        return {
            'cuts': ", ".join(f"{k}={v}" for k, v in data_point['cuts'].items()),
            'mean': data_point['mean_val'],
            'yerr': data_point['mean_err'],
        }
    elif plot_type == "GaussianSigma":
        return {
            'cuts': ", ".join(f"{k}={v}" for k, v in data_point['cuts'].items()),
            'sigma': data_point['sigma_val'],
            'yerr': data_point['sigma_err'],
        }
    elif plot_type == "RelativeSignalError":
        return {
            'cuts': ", ".join(f"{k}={v}" for k, v in data_point['cuts'].items()),
            'relSigErr': data_point['relSig_val'],
        }


# Function to return the appropriate history dictionary based on plot type
def get_history_dict_based_on_plot_type(plot_type):
    # Select and return the correct history dictionary
    if plot_type == "SignalYield":
        return plot_SignalYield_history
    # Additional conditions for other plot types (GaussianMean, GaussianSigma, RelativeSignalError)
    elif plot_type == "GaussianMean":
        return plot_GaussianMean_history
    elif plot_type == "GaussianSigma":
        return plot_GaussianSigma_history
    elif plot_type == "RelativeSignalError":
        return plot_RelativeSignalError_history
    
# Function to update the plot based on user input
def update_plot(clear_plot=True):
    try:
        # Retrieve filter values from the GUI
        energy = float(energy_entry.get())
        asymmetry = float(asymmetry_entry.get())
        chi2 = float(chi2_entry.get())
        delta_r = float(delta_r_entry.get())
        # Filter the data based on user input
        filtered_data = data[(data['Energy'] == energy) & (data['Asymmetry'] == asymmetry) & (data['Chi2'] == chi2) & (data['DeltaR'] == delta_r)]

        if filtered_data.empty:
            messagebox.showerror("Error", "No data matches the specified cut values.")  # Show error if no data found
            return

        # Call the appropriate plot function based on current plot type
        if current_plot_type == "SignalYield":
            plot_SignalYield(filtered_data, clear_plot)
        # Additional conditions for other plot types (GaussianMean, GaussianSigma, RelativeSignalError)
        elif current_plot_type == "GaussianMean":
            plot_GaussianMean(filtered_data, clear_plot)
        elif current_plot_type == "GaussianSigma":
            plot_GaussianSigma(filtered_data, clear_plot)
        elif current_plot_type == "RelativeSignalError":
            plot_RelativeSignalError(filtered_data, clear_plot)
    except ValueError:
        messagebox.showerror("Error", "Invalid cut values entered.") # Show error if invalid input

# Function to overlay new cuts on the existing plot without clearing it
def overlay_new_cuts():
    update_plot(clear_plot=False)  # Calls update_plot with clear_plot set to False

# Function to save the current plot as a PDF file
def save_plot():
    # Opens a file dialog to choose the location and name for saving the plot
    file_path = filedialog.asksaveasfilename(defaultextension=".pdf", filetypes=[("PDF files", "*.pdf")])
    if file_path:
        fig.savefig(file_path)  # Saves the figure to the specified path if a path was given

# Function to exit the program
def exit_program():
    sys.exit()  # Exits the Python interpreter, thereby closing the application

# Function to create the main application window and its components
def create_main_application(root):
    global fig, ax, canvas, energy_entry, asymmetry_entry, chi2_entry, delta_r_entry

    root.deiconify()  # Unhides the root window if it was previously hidden

    # Set the title of the main application window
    root.title("Pi0 Yield Analysis")

    # Set up the main frame in the root window
    frame = ttk.Frame(root)
    frame.pack(fill=tk.BOTH, expand=True)  # Pack the frame to fill the entire window

    # Creating and placing widgets for data input (Energy, Asymmetry, Chi2, DeltaR)
    energy_entry = ttk.Entry(frame)
    energy_entry.grid(row=0, column=1, padx=5, pady=5)
    ttk.Label(frame, text="Energy:").grid(row=0, column=0, padx=5, pady=5)

    asymmetry_entry = ttk.Entry(frame)
    asymmetry_entry.grid(row=1, column=1, padx=5, pady=5)
    ttk.Label(frame, text="Asymmetry:").grid(row=1, column=0, padx=5, pady=5)

    chi2_entry = ttk.Entry(frame)
    chi2_entry.grid(row=2, column=1, padx=5, pady=5)
    ttk.Label(frame, text="Chi2:").grid(row=2, column=0, padx=5, pady=5)

    delta_r_entry = ttk.Entry(frame)
    delta_r_entry.grid(row=3, column=1, padx=5, pady=5)
    ttk.Label(frame, text="DeltaR:").grid(row=3, column=0, padx=5, pady=5)

    # Creating and placing buttons for various functionalities (Update, Overlay, Save, Exit)
    update_button = ttk.Button(frame, text="Update Plot", command=lambda: update_plot(clear_plot=True))
    update_button.grid(row=4, column=0, columnspan=2, padx=5, pady=5)

    overlay_button = ttk.Button(frame, text="Overlay New Cuts", command=overlay_new_cuts)
    overlay_button.grid(row=5, column=0, columnspan=2, padx=5, pady=5)

    save_button = ttk.Button(frame, text="Save Plot as PDF", command=save_plot)
    save_button.grid(row=6, column=0, columnspan=2, padx=5, pady=5)

    exit_button = ttk.Button(frame, text="Exit Program", command=exit_program)
    exit_button.grid(row=7, column=0, columnspan=2, padx=5, pady=5)

    # Setting up the plot area
    fig, ax = plt.subplots()
    canvas = FigureCanvasTkAgg(fig, master=frame)
    canvas.get_tk_widget().grid(row=8, column=0, columnspan=2, padx=5, pady=5)
    annot = ax.annotate('', xy=(0, 0), xytext=(20, 20), textcoords="offset points",
                        bbox=dict(boxstyle="round", fc="w"), arrowprops=dict(arrowstyle="->"))
    annot.set_visible(False)
    fig.canvas.mpl_connect('pick_event', lambda event: on_click(event, root))

    # Setting up the toolbar for the plot
    toolbar_frame = ttk.Frame(root)
    toolbar_frame.pack(fill=tk.X, expand=True)
    toolbar = NavigationToolbar2Tk(canvas, toolbar_frame)
    toolbar.update()
    canvas.draw()

# Function to create an initial window for selecting the plot type
def create_initial_window(root):
    # Create a new top-level window on top of 'root'
    initial_window = tk.Toplevel(root)
    initial_window.title("Choose Plot Type")  # Set title of the window

    # Function to set the global plot type and open the main application
    def set_plot_type_and_continue(plot_type):
        global current_plot_type  # Declare global variable to store the plot type
        current_plot_type = plot_type  # Set the selected plot type
        initial_window.destroy()  # Close the initial window
        create_main_application(root)  # Open the main application window

    # Buttons for selecting the plot type. Clicking these will set the plot type.
    ttk.Button(initial_window, text="Signal Yield", command=lambda: set_plot_type_and_continue("SignalYield")).pack()
    ttk.Button(initial_window, text="Relative Signal Error", command=lambda: set_plot_type_and_continue("RelativeSignalError")).pack()
    ttk.Button(initial_window, text="Gaussian Mean", command=lambda: set_plot_type_and_continue("GaussianMean")).pack()
    ttk.Button(initial_window, text="Gaussian Sigma", command=lambda: set_plot_type_and_continue("GaussianSigma")).pack()

# Main function to initiate the Tkinter application
def main():
    global fig, ax, canvas  # Declare these as global for access throughout the application

    root = tk.Tk()  # Create the root window
    root.withdraw()  # Hide the root window initially
    create_initial_window(root)  # Call to create the initial window for plot type selection
    root.mainloop()  # Start the Tkinter event loop

# Entry point of the Python script
if __name__ == "__main__":
    main()  # Execute the main function
