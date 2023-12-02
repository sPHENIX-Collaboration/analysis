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

matplotlib.use('TkAgg')

# Load data
data = pd.read_csv('/Users/patsfan753/Desktop/Desktop/AnalyzePi0s_Final/dataOutput/PlotByPlotOutput.csv')

# Initialize the maximum y-value
global_max_y = 0
plot_update_count = 0
# Function to create the legend only once
def init_legend():
    # Define labels, colors, and markers for different centralities
    labels = ["60-100%", "40-60%", "20-40%", "0-20%"]
    colors = ['red', 'blue', 'lightgreen', 'deeppink']
    markers = ['o', 's', '^', 'v']
    dummy_lines = []

    # Create dummy error bars for each centrality
    for color, marker, label in zip(colors, markers, labels):
        line = ax.errorbar([], [], yerr=[], fmt=marker, color=color, label=label)
        dummy_lines.append(line)

    # Create custom legend handlers for the error bars
    handler_map = {ErrorbarContainer: HandlerErrorbar(yerr_size=0.5)}

    # Create the legend with the custom items and handlers
    ax.legend(handles=dummy_lines, title='Centrality:', handler_map=handler_map)


# Function to plot data
errorbar_containers = []  # List to store error bar containers
all_y_errs = []

def plot_data(filtered_data, clear_plot):
    global global_max_y,  all_y_errs, plot_update_count
    if clear_plot:
        ax.clear()
        ax.set_yscale('log')
        global_max_y = 0
        init_legend()
        errorbar_containers.clear()
        all_y_errs = []  # Reset the list for new plot data
        plot_update_count = 0  # Reset the counter if the plot is cleared
    else:
        plot_update_count += 1  # Increment the counter for each overlay
        
        
    x_points_base = [2.5, 3.5, 4.5]
    x_ticks = [2, 3, 4, 5]
    colors = ['red', 'blue', 'lightgreen', 'deeppink']
    markers = ['o', 's', '^', 'v']
    local_max_y = 0


    print("Starting to plot data...")

    for i, (color, marker) in enumerate(zip(colors, markers)):
        base_jitter_offset = (i - 1.5) * 0.05
        overlay_jitter_offset = plot_update_count * 0.05  # Additional jitter based on the update count
        total_jitter_offset = base_jitter_offset + overlay_jitter_offset

        indices = range(i * 3, (i + 1) * 3)
        x_vals = [x + total_jitter_offset for x in x_points_base]
        y_vals = [filtered_data.iloc[j]['Yield'] for j in indices]
        y_errs = [filtered_data.iloc[j]['YieldError'] for j in indices]
        all_y_errs.extend(y_errs)

        local_max_y = max(local_max_y, max(y_vals) * 10)
        container = ax.errorbar(x_vals, y_vals, yerr=y_errs, fmt=marker, color=color)
        container[0].set_gid(i)
        container[0].set_picker(10)

        print(f"Plotted data with GID: {i}")

        errorbar_containers.append(container)

    if local_max_y > global_max_y:
        global_max_y = local_max_y

    ax.set_ylim(0.1, global_max_y)
    ax.set_xticks(x_ticks)
    ax.set_xlim(1.5, 5.5)
    canvas.draw()

    print("Finished plotting data")



class InfoDialog(simpledialog.Dialog):
    def __init__(self, parent, title, data, point_index):
        self.data = data
        self.point_index = point_index
        super().__init__(parent, title)
    
    def body(self, frame):
        tk.Label(frame, text=f"Point Index: {self.point_index}").pack()  # Display the point index
        tk.Label(frame, text=f"Cut Values: {self.data['cuts']}").pack()
        tk.Label(frame, text=f"Yield: {self.data['yield']:.2f}").pack()
        tk.Label(frame, text=f"Yield Error: {self.data['yerr']:.2f}").pack()
        tk.Label(frame, text=f"Relative Error: {self.data['rel_err']:.2%}").pack()
        return frame

    def buttonbox(self):
        box = tk.Frame(self)
        w = tk.Button(box, text="OK", width=10, command=self.ok, default=tk.ACTIVE)
        w.pack(side=tk.LEFT, padx=5, pady=5)
        self.bind("<Return>", self.ok)
        box.pack()

def on_click(event):
    print("Click event triggered")

    if hasattr(event.artist, 'get_gid'):
        gid = event.artist.get_gid()
        print(f"GID: {gid}")

        if gid is not None and gid < len(errorbar_containers):
            ind = event.ind[0]  # Get the index of the picked point
            print(f"Index of picked point: {ind}")

            # Get x and y values
            x, y = event.artist.get_xdata()[ind], event.artist.get_ydata()[ind]

            # Calculate the actual index for y_errs
            actual_index = gid * 3 + ind
            if actual_index < len(all_y_errs):
                yerr = all_y_errs[actual_index]
            else:
                print("Error index out of range.")
                yerr = 0

            # Calculate relative error
            rel_err = yerr / y if y != 0 else 0

            print(f"x: {x}, y: {y}, yerr: {yerr}, Relative Error: {rel_err}")

            # Prepare data for the dialog
            data = {
                'cuts': f"E={energy_entry.get()}, A={asymmetry_entry.get()}, C={chi2_entry.get()}, D={delta_r_entry.get()}",
                'yield': y,
                'yerr': yerr,
                'rel_err': rel_err,
            }
            InfoDialog(root, "Point Information", data, actual_index)

            print(f"Data to be displayed: {data}")

            # Create and show the dialog
            InfoDialog(root, "Point Information", data)
    else:
        print("Click event did not trigger on a data point.")


# Function to update the plot
def update_plot(clear_plot=True):
    try:
        energy = float(energy_entry.get())
        asymmetry = float(asymmetry_entry.get())
        chi2 = float(chi2_entry.get())
        delta_r = float(delta_r_entry.get())
        filtered_data = data[(data['Energy'] == energy) & (data['Asymmetry'] == asymmetry) & (data['Chi2'] == chi2) & (data['DeltaR'] == delta_r)]
        if filtered_data.empty:
            messagebox.showerror("Error", "No data matches the specified cut values.")
            return
        plot_data(filtered_data, clear_plot)
    except ValueError:
        messagebox.showerror("Error", "Invalid cut values entered.")

# Function to overlay new cuts without clearing the existing plot
def overlay_new_cuts():
    update_plot(clear_plot=False)

# Function to save the current plot as a PDF
def save_plot():
    file_path = filedialog.asksaveasfilename(defaultextension=".pdf",
                                             filetypes=[("PDF files", "*.pdf")])
    if file_path:
        fig.savefig(file_path)


# Function to exit the program
def exit_program():
    sys.exit()

# Create the main window
root = tk.Tk()
root.title("Pi0 Yield Analysis")


frame = ttk.Frame(root)
frame.pack(fill=tk.BOTH, expand=True)

# Entry fields for cut values
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

# Buttons for updating the plot, saving, and exiting
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

# Create an invisible annotation object to be displayed on click
annot = ax.annotate('', xy=(0, 0), xytext=(20, 20), textcoords="offset points",
                    bbox=dict(boxstyle="round", fc="w"), arrowprops=dict(arrowstyle="->"))
annot.set_visible(False)

# Connect the click handler
fig.canvas.mpl_connect('pick_event', on_click)

# Adding the Matplotlib toolbar
toolbar_frame = ttk.Frame(root)
toolbar_frame.pack(fill=tk.X, expand=True)
toolbar = NavigationToolbar2Tk(canvas, toolbar_frame)
toolbar.update()
canvas.draw()

root.mainloop()
