# Import necessary libraries
import tkinter as tk
from tkinter import ttk, messagebox, filedialog, simpledialog
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.lines import Line2D
from matplotlib.container import ErrorbarContainer
from matplotlib.legend_handler import HandlerErrorbar
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2Tk
import matplotlib
import sys
# Configure the backend to use with Tkinter
matplotlib.use('TkAgg')

# Load the dataset into a pandas DataFrame
data = pd.read_csv('/Users/patsfan753/Desktop/Desktop/AnalyzePi0s_Final/dataOutput/PlotByPlotOutput.csv')

# Initialize a global variable to track the maximum y-value in the plot
global_max_y = 0
# Initialize a counter to keep track of the number of updates to the plot
plot_update_count = 0

# Function to create and initialize the plot legend
def init_legend():
    # Define labels, colors, and markers for the legend
    labels = ["60-100%", "40-60%", "20-40%", "0-20%"]
    colors = ['red', 'blue', 'lightgreen', 'deeppink']
    markers = ['o', 's', '^', 'v']
    dummy_lines = [] # List to hold the line objects for the legend

    # Loop through the centralities to create dummy line objects for the legend
    for color, marker, label in zip(colors, markers, labels):
        line = ax.errorbar([], [], yerr=[], fmt=marker, color=color, label=label)
        dummy_lines.append(line)

    # Custom handler map to specify how error bars are drawn in the legend
    handler_map = {ErrorbarContainer: HandlerErrorbar(yerr_size=0.5)}

    # Create the legend with custom handlers and add it to the plot
    ax.legend(handles=dummy_lines, title='Centrality:', handler_map=handler_map)


# Global variables to keep track of the error bars and data history
errorbar_containers = []
plot_data_history = {}
plot_update_count = 0
colors = ['red', 'blue', 'lightgreen', 'deeppink']

# Function to plot data points on the graph
def plot_data(filtered_data, clear_plot):
    # Access global variables that track the maximum Y value of the plot,
    # the history of plotted data, the number of updates made to the plot,
    # and the list of error bar containers (used for interactive elements).
    global global_max_y, plot_data_history, plot_update_count, errorbar_containers
    if clear_plot:
        # If we need to clear the plot (start afresh), we clear the axis,
        # set the y-scale to logarithmic, and reset the maximum Y value and the update count.
        ax.clear()
        ax.set_yscale('log')
        global_max_y = 0
        init_legend()  # Initialize the plot legend
        errorbar_containers.clear()  # Clear any existing interactive elements
        plot_data_history = {}  # Reset the history of plotted data
        plot_update_count = 0  # Reset the number of updates/plots made
    else:
        # If not clearing, increment the plot update count to keep track of overlay iterations.
        plot_update_count += 1

    # Define base X positions for the data points on the plot.
    x_points_base = [2.5, 3.5, 4.5]
    # Define the X-ticks (useful for labeling the axis).
    x_ticks = [2, 3, 4, 5]
    # Define the markers used for different data sets on the plot.
    markers = ['o', 's', '^', 'v']
    # Initialize the local maximum Y value for this iteration of plotting.
    local_max_y = 0

    # Retrieve the current cut values from the GUI's input fields.
    current_cuts = {
        'E': energy_entry.get(),
        'A': asymmetry_entry.get(),
        'C': chi2_entry.get(),
        'D': delta_r_entry.get()
    }
    
    # Loop through each color/marker set to plot each group of points.
    for i, (color, marker) in enumerate(zip(colors, markers)):
        # Calculate the jitter offsets to prevent overlapping of data points for a single set of points and for overlayed points
        base_jitter_offset = (i - 1.5) * 0.05
        overlay_jitter_offset = plot_update_count * 0.08
        total_jitter_offset = base_jitter_offset + overlay_jitter_offset
        
        # Determine the indices for the filtered data that corresponds to the current group.
        indices = range(i * 3, (i + 1) * 3)
        # Calculate the actual X values for each data point by applying the jitter.
        x_vals = [x + total_jitter_offset for x in x_points_base]
        # Retrieve the Y values and Y error values from the filtered data set.
        y_vals = [filtered_data.iloc[j]['Yield'] for j in indices]
        y_errs = [filtered_data.iloc[j]['YieldError'] for j in indices]
        # Plot each point with its corresponding X, Y, and Y error values.
        for j, (x_val, y_val, y_err) in enumerate(zip(x_vals, y_vals, y_errs)):
            # Create a unique ID for each data point based on its position and update count.
            point_id = (plot_update_count, i, j)
            # Store the data point's information in the history dictionary.
            plot_data_history[point_id] = {
                'y_val': y_val, 'y_err': y_err, 'cuts': current_cuts.copy()
            }
            # Plot the data point with an error bar and set its interactive properties.
            container = ax.errorbar(x_val, y_val, yerr=y_err, fmt=marker, color=color)
            container[0].set_gid(point_id)  # Assign a unique group ID (gid) for interaction.
            container[0].set_picker(5)  # Set the sensitivity of the mouse click detection.
            errorbar_containers.append(container)  # Add the container to the list for reference.

        # Update the local maximum Y value if the new points have a higher value.
        local_max_y = max(local_max_y, max(y_vals) * 10)
        
    # Update the global maximum Y value if the local maximum is greater.
    if local_max_y > global_max_y:
        global_max_y = local_max_y
        
    # Set the plot's Y limits, X ticks, and X limits.
    ax.set_ylim(0.1, global_max_y)
    ax.set_xticks(x_ticks)
    ax.set_xlim(1.5, 5.5)
    # Set the plot title and axis labels.
    ax.set_title(r"$\pi^0$ Yield")
    ax.set_xlabel(r"$\pi^0$ pT")
    ax.set_ylabel(r"$\pi^0$ Signal Yield")
    canvas.draw()



# Define a class that extends the simpledialog.Dialog, which will create a popup window.
class InfoDialog(simpledialog.Dialog):
    # Constructor for the class.
    def __init__(self, parent, title, data, point_index):
        # Store the provided data and point index in the instance.
        self.data = data
        self.point_index = point_index
        # Initialize the superclass with the parent window and the title.
        super().__init__(parent, title)
    
    # This method sets up the body of the dialog window.
    def body(self, frame):
        # Create and pack labels to the dialog window frame, displaying the data.
        tk.Label(frame, text=f"Point Index: {self.point_index}").pack()  # Display the index of the data point
        tk.Label(frame, text=f"Cut Values: {self.data['cuts']}").pack()  # Display the cut values used for this point
        tk.Label(frame, text=f"Yield: {self.data['yield']:.2f}").pack()  # Display the yield value
        tk.Label(frame, text=f"Yield Error: {self.data['yerr']:.2f}").pack()  # Display the error in yield
        tk.Label(frame, text=f"Relative Error: {self.data['rel_err']:.2%}").pack()  # Display the relative error as a percentage
        # Return the frame containing the labels.
        return frame

    # This method creates the button box at the bottom of the dialog.
    def buttonbox(self):
        # Create a frame to hold the buttons.
        box = tk.Frame(self)
        # Create the OK button with the command to execute when clicked.
        w = tk.Button(box, text="OK", width=10, command=self.ok, default=tk.ACTIVE)
        # Pack the button into the box frame.
        w.pack(side=tk.LEFT, padx=5, pady=5)
        # Bind the Return key to the OK button action.
        self.bind("<Return>", self.ok)
        # Pack the box frame into the dialog window.
        box.pack()


# Function to handle click events on the plot.
def on_click(event):
    # Check if the clicked artist (matplotlib object) has a group id (gid).
    if hasattr(event.artist, 'get_gid'):
        # Retrieve the group id.
        point_id = event.artist.get_gid()
        # Check if the point clicked is recorded in the data history.
        if point_id in plot_data_history:
            # Get the index of the clicked point.
            ind = event.ind[0]
            # Retrieve the data for the clicked point from the history.
            data_point = plot_data_history[point_id]
            # Extract the x and y values of the clicked point.
            x, y = event.artist.get_xdata()[ind], event.artist.get_ydata()[ind]
            # Construct the data dictionary for the dialog.
            data = {
                'cuts': ", ".join(f"{k}={v}" for k, v in data_point['cuts'].items()),  # Format the cut values into a string.
                'yield': y,  # Include the yield value.
                'yerr': data_point['y_err'],  # Include the yield error.
                'rel_err': (data_point['y_err'] / y) if y != 0 else 0  # Calculate and include the relative error.
            }
            # Create and show the info dialog with the constructed data.
            InfoDialog(root, "Point Information", data, ind)
        else:
            # Print an error message if the data for the clicked point is not found.
            print("Data for this point is not found.")
            
# Function to update the plot with the current cut values entered by the user.
def update_plot(clear_plot=True):
    try:
        # Try to convert the entry values to float, if conversion fails, catch the ValueError.
        energy = float(energy_entry.get())  # Get energy cut value from the entry field.
        asymmetry = float(asymmetry_entry.get())  # Get asymmetry cut value from the entry field.
        chi2 = float(chi2_entry.get())  # Get chi2 cut value from the entry field.
        delta_r = float(delta_r_entry.get())  # Get DeltaR cut value from the entry field.
        # Filter the dataset based on the cut values entered by the user.
        filtered_data = data[(data['Energy'] == energy) &
                             (data['Asymmetry'] == asymmetry) &
                             (data['Chi2'] == chi2) &
                             (data['DeltaR'] == delta_r)]
        # Check if the filtered data is empty. If it is, show an error dialog.
        if filtered_data.empty:
            messagebox.showerror("Error", "No data matches the specified cut values.")
            return  # Exit the function if no data is found.
        # Call the function to plot data with the filtered dataset.
        plot_data(filtered_data, clear_plot)
    except ValueError:
        # Show an error dialog if the values entered cannot be converted to float.
        messagebox.showerror("Error", "Invalid cut values entered.")

# Function to overlay new cuts onto the existing plot without clearing it.
def overlay_new_cuts():
    update_plot(clear_plot=False)  # Call update_plot with clear_plot set to False.


# Function to save the current plot as a PDF file.
def save_plot():
    # Open a file dialog to specify the file path for saving. Default to .pdf extension.
    file_path = filedialog.asksaveasfilename(defaultextension=".pdf",
                                             filetypes=[("PDF files", "*.pdf")])
    if file_path:  # Check if a file path was provided.
        fig.savefig(file_path)  # Save the current figure to the specified file path.

# Function to exit the program gracefully.
def exit_program():
    sys.exit()  # Exit the program.

# Create the main window for the application with the title 'Pi0 Yield Analysis'.
root = tk.Tk()
root.title("Pi0 Yield Analysis")

# Create a frame to hold the UI elements and add it to the main window.
frame = ttk.Frame(root)
frame.pack(fill=tk.BOTH, expand=True)

# Create entry fields for the cut values and add them to the frame with labels.
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

# Create buttons for updating the plot, overlaying cuts, saving the plot, and exiting the program.
update_button = ttk.Button(frame, text="Update Plot", command=lambda: update_plot(clear_plot=True))
update_button.grid(row=4, column=0, columnspan=2, padx=5, pady=5)

overlay_button = ttk.Button(frame, text="Overlay New Cuts", command=overlay_new_cuts)
overlay_button.grid(row=5, column=0, columnspan=2, padx=5, pady=5)

save_button = ttk.Button(frame, text="Save Plot as PDF", command=save_plot)
save_button.grid(row=6, column=0, columnspan=2, padx=5, pady=5)

exit_button = ttk.Button(frame, text="Exit Program", command=exit_program)
exit_button.grid(row=7, column=0, columnspan=2, padx=5, pady=5)

# Set up the plot area in the UI frame.
fig, ax = plt.subplots()

canvas = FigureCanvasTkAgg(fig, master=frame)
canvas.get_tk_widget().grid(row=8, column=0, columnspan=2, padx=5, pady=5)

# Create an annotation object for displaying information when clicking on the plot. It is initially hidden.
annot = ax.annotate('', xy=(0, 0), xytext=(20, 20), textcoords="offset points",
                    bbox=dict(boxstyle="round", fc="w"), arrowprops=dict(arrowstyle="->"))
annot.set_visible(False)

# Connect the click event handler to the figure's canvas.
fig.canvas.mpl_connect('pick_event', on_click)

# Add the Matplotlib toolbar to the toolbar frame for interaction with the plot.
toolbar_frame = ttk.Frame(root)
toolbar_frame.pack(fill=tk.X, expand=True)
toolbar = NavigationToolbar2Tk(canvas, toolbar_frame)
toolbar.update()

# Draw the initial state of the canvas.
canvas.draw()

# Start the main loop of the Tkinter GUI application.
root.mainloop()
