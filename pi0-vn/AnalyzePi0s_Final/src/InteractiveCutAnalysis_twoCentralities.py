import tkinter as tk  # For creating GUI elements
from tkinter import ttk, messagebox, filedialog, simpledialog  # Additional GUI components and dialogs
import pandas as pd  # For data handling and manipulation
import matplotlib  # For plotting graphs
from matplotlib.patches import Rectangle
matplotlib.use('TkAgg')  # Setting the backend of matplotlib to TkAgg for GUI integration
import matplotlib.pyplot as plt  # For creating and manipulating plots
from matplotlib.lines import Line2D  # For creating line objects in matplotlib
from matplotlib.container import ErrorbarContainer  # For handling error bars in plots
from matplotlib.legend_handler import HandlerErrorbar  # For custom legend handling with error bars
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2Tk  # Tkinter integration with matplotlib
import sys  # For system-specific parameters and functions
import itertools
import numpy as np
import subprocess
import webbrowser
import os
from PIL import Image
from pdf2image import convert_from_path
from PyPDF2 import PdfReader, PdfWriter
from reportlab.pdfgen.canvas import Canvas as ReportLabCanvas
from reportlab.lib.pagesizes import letter, landscape
import shlex  # For safely splitting the command string
import fitz  # PyMuPDF
import io
import math

data = pd.read_csv('/Users/patsfan753/Desktop/Desktop/AnalyzePi0s_Final/dataOutput/PlotByPlotOutputJamiesUpdatedMasking.csv')

# Dictionary to maintain the visibility state of different centrality categories in the plot
centrality_visibility = {"40-60%": True, "20-40%": True}

# List of labels corresponding to different centrality categories
labels = ["40-60%", "20-40%"]

centrality_categories = {
    '60-100%': [0, 1, 2, 3, 4, 5],
    '20-40%': [6, 7, 8, 9, 10, 11],
}
pt_categories = {
#Can change pT label output below
    '2.0 ≤ pT < 2.5': [0, 6],
    '2.5 ≤ pT < 3.0': [1, 7],
    '3.0 ≤ pT < 3.5': [2, 8],
    '3.5 ≤ pT < 4.0': [3, 9],
    '4.0 ≤ pT < 4.5': [4, 10],
    '4.5 ≤ pT < 5.0': [5, 11]
}
# Define marker styles for each centrality category.
markers = ['s', '^']

# List of colors used for differentiating data points in the plot based on centrality categories
colors = ['red', 'blue']

# Define base x-coordinates for the plot points.
x_points_base = [2.25, 2.75, 3.25, 3.75, 4.25, 4.75]

# Set x-axis ticks.
x_ticks = [2, 2.5, 3, 3.5, 4, 4.5, 5]


# Global variables to hold the maximum Y-values for different plot types; used for scaling the plot
global_max_y_signalYield = 0
global_max_y_GaussianMean = 0
global_min_y_GaussianMean = 0
global_max_y_GaussianSigma = 0
global_min_y_GaussianSigma = 0
global_max_y_SBratio = 0
global_min_y_SBratio = 0
global_max_y_RelativeSignalError = 0

# Counters for the total number of points plotted in different plot types
total_point_count_SignalYield = 0
total_point_count_GaussianMean = 0
total_point_count_GaussianSigma = 0
total_point_count_SBratio = 0
total_point_count_RelativeSignalError = 0

# Lists to store containers (plot elements) for different plot types
errorbar_containers_SignalYield = []
errorbar_containers_GaussianMean = []
errorbar_containers_GaussianSigma = []
errorbar_containers_SBratio = []

# Dictionaries to store the history of plotted data points for each plot type
plot_SignalYield_history = {}
plot_GaussianMean_history = {}
plot_GaussianSigma_history = {}
plot_SBratio_history = {}
plot_RelativeSignalError_history = {}

# Counters to keep track of the number of updates made to each plot type
plot_update_count_SignalYield = 0
plot_update_count_GaussianMean = 0
plot_update_count_GaussianSigma = 0
plot_update_count_SBratio = 0
plot_update_count_RelativeSignalError = 0

# To initalize plot type choice thats outputted before GUI box
current_plot_type = "SignalYield"

# Global variable to track the transformation state
is_yield_transformed = False

#initialize the legend for a plot, setting up the labels, colors, and markers for different centrality ranges
def init_legend():
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
    global global_max_y_signalYield, plot_SignalYield_history, plot_update_count_SignalYield, errorbar_containers_SignalYield, total_point_count_SignalYield, centrality_visibility, x_points_base, x_ticks
    
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
        indices = range(i * 6, (i + 1) * 6)

        # Calculate x-values, y-values, error values, and S/B ratios for each point.
        x_vals = [x + total_jitter_offset for x in x_points_base]
        y_vals = [filtered_data.iloc[j]['Yield'] for j in indices]
        y_errs = [filtered_data.iloc[j]['YieldError'] for j in indices]
        S_B_ratios = [filtered_data.iloc[j]['S/B'] for j in indices]
        numEntries = [filtered_data.iloc[j]['NumEntry'] for j in indices]

        # Plot each data point with the corresponding error bar.
        for j, (x_val, y_val, y_err, S_B_ratio, numEntry) in enumerate(zip(x_vals, y_vals, y_errs, S_B_ratios, numEntries)):
            index = i * 3 + j  # Calculate the specific index for the point.
            point_id = (plot_update_count_SignalYield, i, j)  # Unique identifier for each point.
            
            # Store data about the point in the history dictionary.
            plot_SignalYield_history[point_id] = {
                'y_val': y_val, 'y_err': y_err, 'cuts': current_cuts.copy(),
                'centrality': labels[i], 'csv_index': index, 'S_B_ratio': S_B_ratio, 'numEntry': numEntry
            }

            # Create an error bar container for each point and add it to the list.
            container = ax.errorbar(x_val, y_val, yerr=y_err, fmt=marker, color=color)
            container[0].set_gid(point_id)  # Set a group ID for the container.
            container[0].set_picker(5)  # Enable picking on the plot point.
            errorbar_containers_SignalYield.append(container)

            # Increment the count of total points plotted.
            total_point_count_SignalYield += 1
            
        # Update local max y-value based on the plotted data.
        local_max_y = max(local_max_y, max(y_vals) * 50)
        
    # Update global max y-value if the local max is greater.
    if local_max_y > global_max_y_signalYield:
        global_max_y_signalYield = local_max_y
        
    #Set plot limits/labels
    ax.set_ylim(0.1, global_max_y_signalYield)
    ax.set_xticks(x_ticks)
    ax.set_xlim(2.0, 5.0)
    ax.set_ylabel(r"$\pi^0$ Signal Yield")
    ax.set_xlabel(r"$\pi^0$ pT [GeV]")
    ax.set_ylabel(r"$\pi^0$ Signal Yield")
    # Set the title for the plot
    ax.set_title(r"$\pi^0$ Signal Yield over $p_T$")

    #Redraw canvas with new data
    canvas.draw()
    


#Interactive plot of gaussian mean data filled with gaussian mean error
def plot_GaussianMean(filtered_data, clear_plot):
    # Access global variables to modify within the function.
    global global_max_y_GaussianMean, global_min_y_GaussianMean, plot_GaussianMean_history, plot_update_count_GaussianMean, errorbar_containers_GaussianMean, total_point_count_GaussianMean, centrality_visibility, x_points_base, x_ticks

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
        indices = range(i * 6, (i + 1) * 6)

        # Calculate x-values, mean values, errors, and S/B ratios for each point.
        x_vals = [x + total_jitter_offset for x in x_points_base]
        mean_vals = [filtered_data.iloc[j]['GaussMean'] for j in indices]
        mean_errs = [filtered_data.iloc[j]['GaussMeanError'] for j in indices]
        S_B_ratios = [filtered_data.iloc[j]['S/B'] for j in indices]
        numEntries = [filtered_data.iloc[j]['NumEntry'] for j in indices]

        # Plot each data point with the corresponding error bar.
        for j, (x_val, mean_val, mean_err, S_B_ratio, numEntry) in enumerate(zip(x_vals, mean_vals, mean_errs, S_B_ratios, numEntries)):
            index = i * 3 + j  # Calculate the specific index for each point.
            point_id = (plot_update_count_GaussianMean, i, j)  # Unique identifier for the point.
            
            # Store point data in the history dictionary.
            plot_GaussianMean_history[point_id] = {
                'mean_val': mean_val, 'mean_err': mean_err, 'cuts': current_cuts.copy(),
                'centrality': labels[i], 'csv_index': index, 'S_B_ratio': S_B_ratio, 'numEntry': numEntry
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
    # Change the font color to red, remove the background box, and adjust vertical alignment
    ax.text(1.6, pi0_mass, r'$\pi^0$ mass', color='red', va='bottom', ha='left', fontsize=10, bbox=dict(facecolor='none', edgecolor='none'))


    # Set x-axis properties.
    ax.set_xticks(x_ticks)
    ax.set_xlim(1.5, 5.5)

    # Add titles and labels to the plot.
    ax.set_title(r"Gaussian Mean Values vs $\pi^0$ $p_T$")
    ax.set_xlabel(r"$\pi^0$ $p_T$ [GeV]")
    ax.set_ylabel(r"Gaussian Mean")

    # Redraw the canvas with the updated plot.
    canvas.draw()


def plot_GaussianSigma(filtered_data, clear_plot):
    # Access and modify global variables within the function.
    global global_max_y_GaussianSigma, global_min_y_GaussianSigma, plot_GaussianSigma_history, plot_update_count_GaussianSigma, errorbar_containers_GaussianSigma, total_point_count_GaussianSigma, centrality_visibility, x_points_base, x_ticks

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
        indices = range(i * 6, (i + 1) * 6)

        # Calculate x-values, sigma values, errors, and S/B ratios for each point.
        x_vals = [x + total_jitter_offset for x in x_points_base]
        sigma_vals = [filtered_data.iloc[j]['GaussSigma'] for j in indices]
        sigma_errs = [filtered_data.iloc[j]['GaussSigmaError'] for j in indices]
        S_B_ratios = [filtered_data.iloc[j]['S/B'] for j in indices]
        numEntries = [filtered_data.iloc[j]['NumEntry'] for j in indices]

        # Plot each data point with an error bar.
        for j, (x_val, sigma_val, sigma_err, S_B_ratio, numEntry) in enumerate(zip(x_vals, sigma_vals, sigma_errs, S_B_ratios, numEntries)):
            index = i * 3 + j  # Determine the specific index for each point.
            point_id = (plot_update_count_GaussianSigma, i, j)  # Create a unique identifier for the point.
            
            # Store point data in the history dictionary.
            plot_GaussianSigma_history[point_id] = {
                'sigma_val': sigma_val, 'sigma_err': sigma_err, 'cuts': current_cuts.copy(),
                'centrality': labels[i], 'csv_index': index, 'S_B_ratio': S_B_ratio, 'numEntry': numEntry
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
    ax.set_xlabel(r"$\pi^0$ $p_T$ [GeV]")
    ax.set_ylabel(r"Gaussian Sigma")

    # Redraw the canvas with the updated plot.
    canvas.draw()


def plot_SBratio(filtered_data, clear_plot):
    # Access and modify global variables within the function.
    global global_max_y_SBratio, global_min_y_SBratio, plot_SBratio_history, plot_update_count_SBratio, errorbar_containers_SBratio, total_point_count_SBratio, centrality_visibility, x_points_base, x_ticks

    # Clear the existing plot and reset variables if clear_plot is True.
    if clear_plot:
        ax.clear()  # Clears the current axes for a new plot.
        global_max_y_SBratio = 0  # Resets the maximum y-value for Gaussian Sigma.
        global_min_y_SBratio = 0
        init_legend()  # Initializes the legend for the plot.
        errorbar_containers_SBratio.clear()  # Clears existing error bars.
        plot_SBratio_history = {}  # Resets the history for Gaussian Sigma data.
        plot_update_count_SBratio = 0  # Resets the overlay count.
    else:
        plot_update_count_SBratio += 1  # Increment overlay count for each update.

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
        overlay_jitter_offset = plot_update_count_SBratio * 0.08
        total_jitter_offset = base_jitter_offset + overlay_jitter_offset

        # Determine indices for data points in this category.
        indices = range(i * 6, (i + 1) * 6)

        # Calculate x-values, sigma values, errors, and S/B ratios for each point.
        x_vals = [x + total_jitter_offset for x in x_points_base]
        sb_vals = [filtered_data.iloc[j]['S/B'] for j in indices]
        sb_errs = []
        for j, sb_err_value in enumerate([filtered_data.iloc[j]['S/Berror'] for j in indices]):
            if sb_err_value < 0:
                print(f"Negative S/B error value corrected to 0 for index {indices[j]} with cuts: {current_cuts}")
                sb_errs.append(0)
            else:
                sb_errs.append(sb_err_value)
                
        # Plot each data point with an error bar.
        for j, (x_val, sb_val, sb_err) in enumerate(zip(x_vals, sb_vals, sb_errs)):
            index = i * 3 + j  # Determine the specific index for each point.
            point_id = (plot_update_count_SBratio, i, j)  # Create a unique identifier for the point.
            
            # Store point data in the history dictionary.
            plot_SBratio_history[point_id] = {
                'sb_val': sb_val, 'sb_err': sb_err, 'cuts': current_cuts.copy(),
                'centrality': labels[i], 'csv_index': index
            }

            # Create an error bar container for the point and add it to the list.
            container = ax.errorbar(x_val, sb_val, yerr=sb_err, fmt=marker, color=color)
            container[0].set_gid(point_id)  # Assign group ID to the container.
            container[0].set_picker(5)  # Enable picking on the plot point.
            errorbar_containers_SBratio.append(container)

            # Increment the total point count.
            total_point_count_SBratio += 1

        # Update local maximum and minimum y-values based on plotted data.
        max_y_val_with_err = max(sb_val + sb_err for sb_val, sb_err in zip(sb_vals, sb_errs))
        min_y_val_with_err = min(sb_val - sb_err for sb_val, sb_err in zip(sb_vals, sb_errs))
        local_max_y = max(local_max_y, max_y_val_with_err)
        local_min_y = min(local_min_y, min_y_val_with_err)

    # Update global maximum and minimum y-values for Gaussian Sigma.
    if local_max_y > global_max_y_SBratio:
        global_max_y_SBratio = local_max_y
    if local_min_y < global_min_y_SBratio:
        global_min_y_SBratio = local_min_y
    else:
        global_min_y_SBratio = 0  # Default lower bound.

    # Set y-axis limits with buffer factors for better visualization.
    upper_buffer_factor = 1.5
    lower_buffer_factor = 0.1
    ax.set_ylim(global_min_y_SBratio - (global_min_y_SBratio * lower_buffer_factor),
                global_max_y_SBratio * upper_buffer_factor)

    # Set x-axis properties.
    ax.set_xticks(x_ticks)
    ax.set_xlim(1.5, 5.5)

    # Add titles and labels to the plot.
    ax.set_title(r"Signal To Background Ratio vs $\pi^0$ pT")
    ax.set_xlabel(r"$\pi^0$ $p_T$ [GeV]")
    ax.set_ylabel(r"S/B")

    # Redraw the canvas with the updated plot.
    canvas.draw()

# Global dictionary to map each plotted point to its line index on the plot.
# Have to go about it a bit differently since not using error bars
line_index_mapping = {}
def plot_RelativeSignalError(filtered_data, clear_plot):
    # Access and modify global variables within the function.
    global global_max_y_RelativeSignalError, plot_RelativeSignalError_history, plot_update_count_RelativeSignalError, total_point_count_RelativeSignalError, centrality_visibility, line_index_mapping, x_points_base, x_ticks
    
    # Clear the plot and reset relevant variables if clear_plot is True.
    if clear_plot:
        ax.clear()  # Clears the current axes for a new plot.
        global_max_y_RelativeSignalError = 0  # Resets the maximum y-value for Relative Signal Error.
        init_legend()  # Initializes the legend for the plot.
        plot_RelativeSignalError_history = {}  # Resets the history for Relative Signal Error data.
        plot_update_count_RelativeSignalError = 0  # Resets the overlay count.
    else:
        plot_update_count_RelativeSignalError += 1  # Increment overlay count for each update.

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
        indices = range(i * 6, (i + 1) * 6)
        
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
    ax.set_xlabel(r"$\pi^0$ $p_T$ [GeV]")
    ax.set_ylabel(r"Relative Signal Error")
    
    # Redraw the canvas with the updated plot.
    canvas.draw()


def summarize_index(root, plot_type):
    global current_plot_type
    # Determine the appropriate title based on the plot type
    # Determine the appropriate title based on the plot type
    if current_plot_type == "SignalYield":
        title = "Summarize Yield For Specific Index"
    elif current_plot_type == "GaussianSigma":
        title = "Summarize Sigma For Specific Index"
    elif current_plot_type == "SBratio":
        title = "Summarize S/B For Specific Index"
    else:
        title = "Summarize Data For Specific Index"  # Default title
    index_window = tk.Toplevel(root)
    index_window.title(title)

    tk.Label(index_window, text="Enter Index:").pack(side=tk.LEFT)
    index_entry = tk.Entry(index_window)
    index_entry.pack(side=tk.LEFT)
    # Pass the current plot type to the plot_index_data function when the button is pressed
    submit_button = ttk.Button(index_window, text="Submit",
                               command=lambda: plot_index_data(index_entry.get(), plot_type))
    submit_button.pack(side=tk.LEFT)



def plot_index_data(index_str, history_dict):
    try:
        index = int(index_str)
        history_dict = get_history_dict_based_on_plot_type(current_plot_type)
        index_data = [data for point_id, data in history_dict.items() if data['csv_index'] == index]

        if not index_data:
            messagebox.showerror("Error", f"No data found for index {index}")
            return
            
        # Create the main figure and axis for the plot
        fig, ax = plt.subplots(figsize=(15, 8))
        plt.subplots_adjust(right=0.7)  # Adjusting the subplot to make room for the legend
        plt.style.use('ggplot')
        root = tk.Tk()
        root.geometry("1600x900")
        canvas = FigureCanvasTkAgg(fig, master=root)
        canvas.draw()

        # Adjust the placement of the canvas and toolbar
        canvas.get_tk_widget().pack(side=tk.TOP, fill=tk.BOTH, expand=1)
        toolbar = NavigationToolbar2Tk(canvas, root)
        toolbar.update()
        canvas._tkcanvas.pack(side=tk.TOP, fill=tk.BOTH, expand=1)


        # Prepare the data for plotting
        cut_combinations = {}
        cut_labels_map = {}
        for i, data in enumerate(index_data):
            cuts = {k: v for k, v in sorted(data['cuts'].items())}
            cut_combinations[tuple(cuts.values())] = i + 1
            cut_labels_map[i + 1] = cuts  # Map the index to the cuts



        # Determine the plotting keys and labels
        plot_type_to_keys = {
            "SignalYield": ('y_val', 'y_err', 'Signal Yield'),
            "GaussianSigma": ('sigma_val', 'sigma_err', 'Gaussian Sigma'),
            "SBratio": ('sb_val', 'sb_err', 'S/B')
        }
        values_key, errors_key, ylabel = plot_type_to_keys[current_plot_type]


        values = [data[values_key] for data in index_data]
        errors = [data[errors_key] for data in index_data]
        
        
        # Check if transformation should be applied for Signal Yield
        if current_plot_type == "SignalYield" and is_yield_transformed:
            values = [math.sqrt(value) / value if value > 0 else 0 for value in values]

        
        # Create a list of tuples containing the index (assigned index to cut combination, not histogram indices), value, and error
        indexed_data = [(cut_label, value, error) for cut_label, value, error in zip(cut_combinations.values(), values, errors)]

        # Sort this list by 'value' in descending order
        sorted_indexed_data = sorted(indexed_data, key=lambda x: x[1], reverse=True)

        # Terminal output for each index assigned, its corresponding cuts, and the yield value, sorted from greatest to least
        for cut_label, value, error in sorted_indexed_data:
            cut_details = cut_labels_map[cut_label]
            cut_details_str = ', '.join(f"{k}: {v}" for k, v in cut_details.items())
            print(f"Index {cut_label}): Cuts - {cut_details_str}, {current_plot_type} - Value: {value}, Error: {error}")


        # Plot each data point with error bars
        for cut_label, value, error in zip(cut_combinations.values(), values, errors):
            ax.errorbar(cut_label, value, yerr=error, fmt='o', color='blue', ecolor='lightblue', elinewidth=3, capsize=0, alpha=0.7)

        # Set plot title, labels, and ticks
        centrality_label, pt_label = get_centrality_and_pt_range(index)
        ax.set_title(f'{current_plot_type} for Index {index} ({centrality_label}, {pt_label})', fontsize=16)
        ax.set_xlabel('Cut Combinations', fontsize=14)
        ax.set_ylabel(ylabel, fontsize=14)
        ax.set_xticks(range(1, len(cut_combinations) + 1))
        ax.grid(True, linestyle='--', linewidth=0.5, alpha=0.7)

        cut_mapping = {'A': '      Asy', 'C': r'$\chi^2$', 'D': r'  $\Delta R$', 'E': 'E'}


        # Define the headers for the legend
        headers = ['A', 'C', 'D', 'E']  # The abbreviations to be used as headers

        # Create the header row for the legend
        header_row = [cut_mapping[header] for header in headers]

        # Sort the cut combinations by their index for consistent ordering
        sorted_combinations = sorted(cut_labels_map.items(), key=lambda x: x[0])

        # Construct the rows for the legend values under each header
        legend_values = []
        for index, cuts in sorted_combinations:
            # Append a string with the index and a closing parenthesis
            values = [f"{index})"] + [str(cuts[header]) for header in headers]
            legend_values.append(values)
        # Convert the legend values to a NumPy array for column-wise alignment
        legend_array = np.array(legend_values, dtype=str)
        

        # Find the maximum length of the string in each column for alignment
        max_lens = np.vectorize(len)(legend_array).max(axis=0)

        # Create the formatted legend string for each row
        legend_str_list = ['    '.join(header_row)]
        for row in legend_array:
            formatted_row = '    '.join(word.ljust(max_lens[i]) for i, word in enumerate(row))
            legend_str_list.append(formatted_row)

        # Create a legend handle for each row
        legend_handles = [Rectangle((0, 0), 1, 1, fill=False, edgecolor='none', visible=False) for _ in legend_str_list]

        # Adjust subplot and legend location
        plt.subplots_adjust(right=0.8)  # Adjust subplot to make room for the legend
        # Utilize a monospaced font for the legend
        monospace_font = {'family': 'monospace'}
        plt.rc('font', **monospace_font)

        # Create the legend with custom handles
        leg = ax.legend(legend_handles, legend_str_list, loc='center left', bbox_to_anchor=(1.05, 0.5), fontsize='small', title='Cut Combinations', handlelength=0, handletextpad=0)
        leg.get_frame().set_edgecolor('black')

        # Function to save the plot with a choice of file format
        def save_figure():
            filetypes = [
                ("PDF files", "*.pdf"),
                ("PNG files", "*.png"),
                # Add other file types if needed
            ]
            file_path = filedialog.asksaveasfilename(defaultextension=".pdf", filetypes=filetypes)
            if file_path:
                # Determine the format based on the file extension
                file_format = 'PDF' if file_path.endswith('.pdf') else 'PNG'
                # Save the current figure with high resolution and tight bounding box
                fig.savefig(file_path, dpi=300, format=file_format.lower(), bbox_inches='tight', bbox_extra_artists=[leg])

        save_button = tk.Button(master=root, text="Save as PDF or PNG", command=save_figure)
        save_button.pack(side=tk.BOTTOM)

        root.lift()
        root.attributes('-topmost', True)
        root.after_idle(root.attributes, '-topmost', False)
        tk.mainloop()
        
    except ValueError:
        messagebox.showerror("Error", "Invalid index entered. Please enter a numerical index.")
        
        
# Helper function to get centrality and pT range for the index
def get_centrality_and_pt_range(index):
    for centrality, indices in centrality_categories.items():
        if index in indices:
            for pt_range, pt_indices in pt_categories.items():
                if index in pt_indices:
                    return centrality, pt_range
    return "Unknown Centrality", "Unknown pT"



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
        
        # Display information specific to the type of data plotted.
        if self.plot_type == "SignalYield":
            tk.Label(frame, text=f"S/B: {self.data['S_B_ratio']:.4f}").pack()
            # Display yield and related error for Signal Yield.
            tk.Label(frame, text=f"Yield: {self.data['yield']:.4f}").pack()
            tk.Label(frame, text=f"Yield Error: {self.data['yerr']:.4f}").pack()
            tk.Label(frame, text=f"Relative Error: {self.data['rel_err']:.4%}").pack()
        elif self.plot_type == "GaussianMean":
            tk.Label(frame, text=f"S/B: {self.data['S_B_ratio']:.4f}").pack()
            # Display mean and error for Gaussian Mean.
            tk.Label(frame, text=f"Mean: {self.data['mean']:.7f}").pack()
            tk.Label(frame, text=f"Mean Error: {self.data['yerr']:.7f}").pack()
        elif self.plot_type == "GaussianSigma":
            tk.Label(frame, text=f"S/B: {self.data['S_B_ratio']:.4f}").pack()
            # Display sigma and error for Gaussian Sigma.
            tk.Label(frame, text=f"Sigma: {self.data['sigma']:.7f}").pack()
            tk.Label(frame, text=f"Sigma Error: {self.data['yerr']:.7f}").pack()
        elif self.plot_type == "SBratio":
            tk.Label(frame, text=f"S/B: {self.data['SBratio']:.7f}").pack()
            tk.Label(frame, text=f"S/B Error: {self.data['yerr']:.7f}").pack()
        elif self.plot_type == "RelativeSignalError":
            tk.Label(frame, text=f"S/B: {self.data['S_B_ratio']:.4f}").pack()
            # Display relative signal error for Relative Signal Error.
            tk.Label(frame, text=f"Rel Sig Err: {self.data['relSigErr']:.7f}").pack()
            
        return frame # Return the frame as the body of the dialog.
        
    # Method to create the button box at the bottom of the dialog.
    def buttonbox(self):
        box = tk.Frame(self)

        # OK button
        ok_button = tk.Button(box, text="OK", width=10, command=self.ok, default=tk.ACTIVE)
        ok_button.pack(side=tk.LEFT, padx=5, pady=5)
        
        # Button for opening the invariant mass histogram
        hist_button = tk.Button(box, text="Open Inv Mass Hist", width=20, command=self.open_inv_mass_hist)
        hist_button.pack(side=tk.LEFT, padx=5, pady=5)

        self.bind("<Return>", self.ok)
        box.pack()

    def open_inv_mass_hist(self):
        index = self.data['index']
        cuts = self.data['cuts']
        # Convert cuts from string to dictionary if needed
        cuts_dict = {k: v for k, v in (x.split('=') for x in cuts.split(', '))}
        self.open_histogram(index, cuts_dict)

    @staticmethod
    def open_histogram(index, cuts):
        # Path to the directory containing the histograms
        histogram_path = "/Users/patsfan753/Desktop/Desktop/AnalyzePi0s_Final/plotOutput/InvMassPlots"
        
        def format_cut_value(value):
            try:
                # Convert to a float
                float_value = float(value)
                
                # Check if the value is an integer
                if float_value.is_integer():
                    return str(int(float_value))
                
                # Check if it's a special case like '.75' which needs to be kept with two decimal places
                elif len(value) > 2 and value[2] != '0':
                    return f"{float_value:.2f}"
                
                # Otherwise, format the number with a single decimal place
                else:
                    return f"{float_value:.1f}"
            except ValueError:
                return value

        formatted_cuts = {k: format_cut_value(v) for k, v in cuts.items()}
        
        filename = f"hPi0Mass_{index}_E{formatted_cuts['E']}_Asym{formatted_cuts['A']}_Chi{formatted_cuts['C']}_DeltaR{formatted_cuts['D']}_fit.pdf"
        full_path = os.path.join(histogram_path, filename)

        print(f"Attempting to open histogram PDF: {filename}")
        print(f"Full path: {full_path}")

        if os.path.isfile(full_path):
            try:
                # Execute the open command directly
                result = subprocess.run(['open', full_path], check=True, capture_output=True, text=True)
                print(result.stdout)
            except subprocess.CalledProcessError as e:
                print(f"Failed to open the PDF. Return code: {e.returncode}. Output: {e.output}. Error: {e.stderr}")
                tk.messagebox.showerror("Error", f"Failed to open the PDF using 'open' command. Return code: {e.returncode}. Output: {e.output}. Error: {e.stderr}")
            except Exception as e:
                print(f"An unexpected error occurred: {e}")
                tk.messagebox.showerror("Error", f"An unexpected error occurred: {e}")
        else:
            tk.messagebox.showerror("Error", f"Histogram PDF does not exist at {full_path}.")


# Placeholder for the format_cut_value function
def format_cut_value(value):
    print(f"Formatting value: {value}")
    try:
        # Convert to a float
        float_value = float(value)
        
        # Check if the value is an integer
        if float_value.is_integer():
            return str(int(float_value))
        
        # Check if it's a special case like '.75' which needs to be kept with two decimal places
        elif len(value) > 2 and value[2] != '0':
            return f"{float_value:.2f}"
        
        # Otherwise, format the number with a single decimal place
        else:
            return f"{float_value:.1f}"
    except ValueError:
        return value


def add_labels_and_dividers(page, cell_width, cell_height, centrality_categories, pt_categories, cuts):
    # Define text and line styles
    font_size = 10
    red = (1, 0, 0)
    black = (0, 0, 0)  # Black color for lines
    blue = (0, 0, 1)   # Blue color for the title
    # Calculate the number of rows and columns
    num_rows = len(centrality_categories)
    num_cols = len(pt_categories)

    # Add the horizontal line above the centralities labels
    top_label_y_position = font_size * 3  # Adjust this value as necessary to position above the line
    
    # Draw vertical dividing lines extended to just under the title
    for i in range(num_cols + 1):
        x_line = i * cell_width
        # Start the line a bit above the top_y_label position
        start_y_position = top_label_y_position - font_size  # Adjust as needed to go just under the title
        end_y_position = page.rect.height  # End at the bottom of the page
        page.draw_line((x_line, start_y_position), (x_line, end_y_position), color=black, width=1)


    # Draw horizontal dividing lines
    for i in range(num_rows):
        y_line = i * cell_height
        page.draw_line((0, y_line), (page.rect.width, y_line), color=black, width=1)


    # Add the horizontal line above the centralities labels
    page.draw_line((0, font_size * 2), (page.rect.width, font_size * 2), color=black, width=1)

    # Add the title with the cuts used for the plot
    title = f"E: {cuts['E']}, Asymmetry: {cuts['A']}, delR: {cuts['D']}, and chi2: {cuts['C']}"
    # Center the title on the page
    title_x = (page.rect.width - len(title) * font_size / 2) / 2  # Adjust calculation as needed
    page.insert_text((title_x, font_size), title, fontsize=font_size, color=blue)

    row_label_font_size = 8  # Reduced font size for the row labels

    # Add centrality labels above the top dividing line
    for i, centrality_label in enumerate(centrality_categories.keys()):
        x_position = (i * cell_width) + (cell_width / 2)
        page.insert_text((x_position, top_label_y_position - font_size), centrality_label, fontsize=font_size, color=red)
   
    # Add pT labels to the left of each row
    for i, pt_label in enumerate(pt_categories.keys()):
        # Calculate the y_position to center the label in the middle of the row
        # and shift it down a bit by adding a small y_offset
        y_offset = 15  # Adjust this value as needed for a slight downward shift
        y_position = (i * cell_height) + (cell_height / 2) - (row_label_font_size / 2) + y_offset

        # Adjust the x_position to account for smaller font size and centering
        x_position = cell_width * 0.05  # 5% of the cell width from the left edge of the page

        # Insert the row label text with the smaller font size and adjusted y_position
        page.insert_text((x_position, y_position), pt_label, fontsize=row_label_font_size, color=red, rotate=90)



def generate_histogram_table(cuts):
    histogram_path = "/Users/patsfan753/Desktop/Desktop/AnalyzePi0s_Final/plotOutput/InvMassPlots"
    formatted_cuts = {k: format_cut_value(v) for k, v in cuts.items()}

    # Create a new PDF to place the histograms
    output_filename = "Table_E{}_A{}_Chi{}_DeltaR{}.pdf".format(
        formatted_cuts['E'],
        formatted_cuts['A'],
        formatted_cuts['C'],
        formatted_cuts['D']
    )
    new_pdf = fitz.open()  # Create a new PDF

    # Use A4 landscape dimensions (in points)
    a4_width = 842
    a4_height = 595

    new_page = new_pdf.new_page(pno=-1, width=a4_width, height=a4_height)  # A4 size in points, landscape

    output_path = "/Users/patsfan753/Desktop"

    # Calculate the size and position for each histogram on an A4 landscape
    # Divide by 4 for the columns because there are 4 histograms per row
    cell_width = a4_width / 4

    # Divide by 3 for the rows because there are 3 rows of histograms
    cell_height = a4_height / 3

    # Calculate the number of rows and columns based on the categories
    num_rows = len(centrality_categories)
    num_cols = len(pt_categories)

    # After setting up the new_page, call this function to add the dividing lines and labels
    add_labels_and_dividers(new_page, cell_width, cell_height, centrality_categories, pt_categories, formatted_cuts)


    #Forced row mapping into correct order
    row_mapping = {
        2: 0,
        5: 0,
        8: 0,
        11: 0,
        1: 1,
        4: 1,
        7: 1,
        10: 1,
        0: 2,
        3: 2,
        6: 2,
        9: 2
    }
    # Constants for adjustments
    bottom_padding = .01  # Space from the bottom of the page to the bottom row of images
    inter_row_spacing = .5  # Space between the rows of images

    # Calculate the total height used by the plots and the spacing
    total_plots_height = 3 * cell_height + 2 * -inter_row_spacing  # For three rows and spacing between them
    remaining_space = a4_height - total_plots_height - bottom_padding  # Remaining space on the page

    # Calculate the starting y-position for the bottom row
    bottom_row_start_y = remaining_space + bottom_padding


    for pt_label, pt_indices in pt_categories.items():
        for centrality_label, centrality_indices in centrality_categories.items():
            for centrality_index in centrality_indices:
                if centrality_index in pt_indices:
                    column = pt_indices.index(centrality_index)
                    # Use the row_mapping to determine the correct row based on the index
                    row = row_mapping[centrality_index]


                    # Add an offset to the x_position to move the plot to the right
                    x_offset = cell_width * 0.01  # 10% of the cell width, adjust as needed
                    x_position = column * cell_width + x_offset


                    # Calculate the y_position for each row
                    if row == 2:  # Bottom row
                        y_position = bottom_row_start_y
                    elif row == 1:  # Middle row
                        y_position = bottom_row_start_y + cell_height + inter_row_spacing
                    else:  # Top row
                        y_position = bottom_row_start_y + 2 * (cell_height + inter_row_spacing)



                    filename = f"hPi0Mass_{centrality_index}_E{formatted_cuts['E']}_Asym{formatted_cuts['A']}_Chi{formatted_cuts['C']}_DeltaR{formatted_cuts['D']}_fit.pdf"
                    file_path = os.path.join(histogram_path, filename)

                    if os.path.exists(file_path):
                        src_pdf = fitz.open(file_path)
                        src_page = src_pdf[0]

                        # Extract the first page as an image
                        pix = src_page.get_pixmap()
                        img_data = pix.tobytes("png")
                        img = Image.open(io.BytesIO(img_data))

                        # Rotate the image using PIL
                        rotated_img = img.rotate(0, expand=True)  # 270 degrees to rotate clockwise

                        # Save the rotated image to a bytes buffer
                        img_byte_arr = io.BytesIO()
                        rotated_img.save(img_byte_arr, format='PNG')
                        img_byte_arr = img_byte_arr.getvalue()

                        # Create an image XObject from the rotated image
                        img_xobj = fitz.open("png", img_byte_arr)

                        # Calculate the new scale factors after rotation
                        img_rect = img_xobj[0].rect
                        fitz_scale_factor = min(cell_width / img_rect.width, cell_height / img_rect.height)

                        # Add the image to the PDF page at the correct position and scale
                        new_page.insert_image(fitz.Rect(x_position, y_position, x_position + cell_width, y_position + cell_height), stream=img_byte_arr, keep_proportion=True, overlay=True)

                        src_pdf.close()
                    else:
                        print(f"File does not exist: {filename}")

    # Save the new PDF
    add_labels_and_dividers(new_page, cell_width, cell_height, centrality_categories, pt_categories, formatted_cuts)
    new_pdf.save(os.path.join(output_path, output_filename))
    new_pdf.close()

    
def on_generate_table_click():
    print("Generate table button clicked.")
    # Read the cuts from the user inputs
    cuts = {
        'E': energy_entry.get(),
        'A': asymmetry_entry.get(),
        'C': chi2_entry.get(),
        'D': delta_r_entry.get()
    }
    print(f"Cuts received: {cuts}")
    # Call the function to generate the histogram table
    generate_histogram_table(cuts)
    print("Table generation process completed.")
    
    
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
    elif plot_type == "SBratio":
        return {
            'cuts': ", ".join(f"{k}={v}" for k, v in data_point['cuts'].items()),
            'index': data_point.get('csv_index', 'N/A'),
            'SBratio': data_point['sb_val'],
            'yerr': data_point['sb_err'],
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
    elif plot_type == "SBratio":
        return plot_SBratio_history
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

        # Log the cut values for debugging purposes.
        print(f"Cut values entered - Energy: {energy}, Asymmetry: {asymmetry}, Chi2: {chi2}, DeltaR: {delta_r}")

        # Filter the dataset based on the current cut values.
        filtered_data = data[(data['Energy'] == energy) & (data['Asymmetry'] == asymmetry) & (data['Chi2'] == chi2) & (data['DeltaR'] == delta_r)]
        
        # Log the filtered data for debugging purposes.
        if filtered_data.empty:
            print("Filtered data is empty after applying cuts.")
        else:
            print(f"Filtered data contains {len(filtered_data)} entries after applying cuts.")

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
        elif current_plot_type == "SBratio":
            plot_SBratio(filtered_data, clear_plot)
        elif current_plot_type == "RelativeSignalError":
            plot_RelativeSignalError(filtered_data, clear_plot)
    except ValueError as e:
        # Log the error for debugging purposes.
        print(f"A ValueError occurred: {e}")

        # Show an error message with the exception details.
        messagebox.showerror("Error", f"Invalid cut values entered. Details: {e}")


# Function to overlay new cuts on the existing plot without clearing it.
def overlay_new_cuts():
    # Call update_plot with clear_plot set to False, indicating an overlay is required.
    update_plot(clear_plot=False)
    
def parse_cut_values(cut_string):
    return [x.strip() for x in cut_string.split(',')]
    
# Adjust the cut entries and plot for the first time or overlay based on the clear_plot flag
def apply_cuts_and_plot(energy, asymmetry, chi2, delta_r, clear_plot):
    energy_entry.delete(0, tk.END)
    energy_entry.insert(0, energy)
    asymmetry_entry.delete(0, tk.END)
    asymmetry_entry.insert(0, asymmetry)
    chi2_entry.delete(0, tk.END)
    chi2_entry.insert(0, chi2)
    delta_r_entry.delete(0, tk.END)
    delta_r_entry.insert(0, delta_r)

    if clear_plot:
        update_plot(clear_plot=True)
    else:
        overlay_new_cuts()

# Function to generate all combinations of cuts and plot them
def plot_all_combinations():
    # Parse the cut values from the entry widgets
    energies = parse_cut_values(energy_entry.get())
    asymmetries = parse_cut_values(asymmetry_entry.get())
    chi2s = parse_cut_values(chi2_entry.get())
    delta_rs = parse_cut_values(delta_r_entry.get())
    
    # Generate all combinations of cuts
    all_cut_combinations = itertools.product(energies, asymmetries, chi2s, delta_rs)
    
    # Start with clearing the plot for the first set of cuts
    clear_plot = True
    
    # Iterate over all combinations and apply each one
    for combination in all_cut_combinations:
        apply_cuts_and_plot(*combination, clear_plot=clear_plot)
        # After the first plot, set clear_plot to False so that subsequent plots are overlaid
        clear_plot = False

    # Redraw the canvas with all the new data
    canvas.draw()

def apply_transformation():
    # Accessing global variables that will be modified in this function.
    global is_yield_transformed, plot_SignalYield_history, ax, canvas, global_max_y_signalYield

    # Toggle the state of whether the transformation has been applied.
    # If it was True, it becomes False, and vice versa.
    is_yield_transformed = not is_yield_transformed

    # Initialize variables to track the maximum and minimum transformed y-values.
    # These will be used to adjust the plot's y-axis limits later.
    new_max_y = 0
    new_min_y = float('inf')  # Set to infinity initially.

    # Iterate over each point in the signal yield history.
    for point_id, point_data in plot_SignalYield_history.items():
        # Extract the original y-value (yield) and its error.
        y_val = point_data['y_val']
        y_err = point_data['y_err']

        # Apply the transformation only if the y-value is positive.
        if y_val > 0:
            # Transform the y-value using sqrt(y)/y formula.
            transformed_value = math.sqrt(y_val) / y_val
            # Transform the error using error propagation formula.
            transformed_error = (0.5 * y_err) / (y_val ** 1.5)

            # Update the point data with transformed values.
            point_data['y_val'] = transformed_value
            point_data['y_err'] = transformed_error

            # Update the new maximum and minimum y-values after transformation.
            new_max_y = max(new_max_y, transformed_value)
            new_min_y = min(new_min_y, transformed_value)

            # Update the plot for each point.
            for container in errorbar_containers_SignalYield:
                # Check if the container corresponds to the current point_id.
                if container[0].get_gid() == point_id:
                    # Set the y-data for the main data point (marker/line) to the transformed value.
                    container[0].set_ydata([transformed_value])

                    # Extract the LineCollection object from the tuple (error bars).
                    error_lines = container[2][0]
                    # Retrieve the segments representing the error bars.
                    segments = error_lines.get_segments()
                    # Update each segment (error bar) with the transformed error values.
                    for segment in segments:
                        segment[0][1] = transformed_value - transformed_error  # Lower end of the error bar
                        segment[1][1] = transformed_value + transformed_error  # Upper end of the error bar
                    # Set the updated segments back to the LineCollection object.
                    error_lines.set_segments(segments)

    # Calculate a safe lower limit for the y-axis on a logarithmic scale.
    lower_limit = new_min_y / 10 if new_min_y / 10 > 0 else new_min_y * 10

    # Set the new limits for the y-axis of the plot.
    ax.set_ylim(lower_limit, new_max_y * 10)
    # Update the global variable to reflect the new maximum y-value.
    global_max_y_signalYield = new_max_y
    
    # Update the y-axis label based on the transformation state.
    if is_yield_transformed:
        ax.set_ylabel(r"$\sqrt{\text{Yield}}/\text{Yield}$")
        ax.set_title(r"$\sqrt{\pi^0 \text{Signal Yield}}$/Yield over $p_T$")
    else:
        ax.set_ylabel(r"$\pi^0$ Signal Yield")
        ax.set_title(r"$\pi^0$ Signal Yield over $p_T$")



    # Redraw the canvas to reflect the updates made to the plot.
    canvas.draw_idle()



# Function to save the current plot to a file with a choice of format.
def save_plot():
    # Define the file types that the user can save as.
    filetypes = [
        ("PDF files", "*.pdf"),
        ("PNG files", "*.png"),
        # Add other file types if needed.
    ]
    
    # Open a file dialog to choose the save location and file name.
    file_path = filedialog.asksaveasfilename(defaultextension=".pdf",
                                             filetypes=filetypes)
    if file_path:
        # Determine the format based on the file extension.
        if file_path.endswith('.pdf'):
            file_format = 'PDF'
        elif file_path.endswith('.png'):
            file_format = 'PNG'
        else:
            return  # If the file format is not recognized, return without saving.

        # Save the current figure to that path.
        fig.savefig(file_path, format=file_format.lower())


# Function to exit the program.
def exit_program():
    # Terminate the program execution.
    sys.exit()

    
    
# Function to create the main application window with all its widgets.
def create_main_application(root):
    # Define global variables that will be used in this function.
    global fig, ax, canvas, energy_entry, asymmetry_entry, chi2_entry, delta_r_entry, current_plot_type
    
    # Make the root window visible and set its title.
    root.deiconify()
    root.title("Pi0 Yield Analysis")
    
    # Set a style for the application
    style = ttk.Style()
    style.configure('TButton', font=('Helvetica', 12, 'bold'), borderwidth='1',
                    background="#00b4db", relief="raised")
    style.configure('TFrame', background='light gray', relief='sunken', borderwidth=5)

    style.map('TButton',
              background=[('active', '#0083b0')],
              foreground=[('active', 'white')])

    style.configure('TLabel', font=('Helvetica', 10, 'bold'), background='light gray')
    style.configure('TEntry', font=('Helvetica', 10), borderwidth='1')
    style.configure('TLabelframe', font=('Helvetica', 10, 'bold'), borderwidth='2')
    style.configure('TLabelframe.Label', font=('Helvetica', 10, 'bold'))
    style.configure('TCheckbutton', font=('Helvetica', 10, 'bold'))

    # Set the theme for the application
    style.theme_use('clam')

    # Set the background color for the root window if desired
    root.configure(background='light gray')
    
    
    # Create two main frames: one for the left side and one for the right side.
    left_frame = ttk.Frame(root)
    # When packing the left_frame, add some left padding to create a gap
    left_frame.pack(side=tk.LEFT, fill=tk.Y, padx=(10, 0))  # Added padx to create the gap


    # Create a frame for the buttons at the bottom left
    bottom_left_frame = ttk.Frame(root)
    bottom_left_frame.pack(side=tk.BOTTOM, anchor='w', fill=tk.X)


    # Create and place cut entry boxes in the left frame.
    ttk.Label(left_frame, text="Energy:").grid(row=0, column=0, sticky='e', pady=(10, 0))
    energy_entry = ttk.Entry(left_frame, width=10)
    energy_entry.grid(row=0, column=1, sticky='w', pady=(10, 0))

    ttk.Label(left_frame, text="Asymmetry:").grid(row=1, column=0, sticky='e', pady=(10, 0))
    asymmetry_entry = ttk.Entry(left_frame, width=10)
    asymmetry_entry.grid(row=1, column=1, sticky='w', pady=(10, 0))

    ttk.Label(left_frame, text="Chi2:").grid(row=2, column=0, sticky='e', pady=(10, 0))
    chi2_entry = ttk.Entry(left_frame, width=10)
    chi2_entry.grid(row=2, column=1, sticky='w', pady=(10, 0))

    ttk.Label(left_frame, text="DeltaR:").grid(row=3, column=0, sticky='e', pady=(10, 0))
    delta_r_entry = ttk.Entry(left_frame, width=10)
    delta_r_entry.grid(row=3, column=1, sticky='w', pady=(10, 0))


    # Add the buttons with added padding.
    update_button = ttk.Button(left_frame, text="Apply First Cuts/Renew Points", style='TButton', command=lambda: update_plot(clear_plot=True))
    update_button.grid(row=4, column=0, columnspan=2, sticky='ew', pady=(80, 0))

    overlay_button = ttk.Button(left_frame, text="Overlay New Cuts", command=overlay_new_cuts)
    overlay_button.grid(row=5, column=0, columnspan=2, sticky='ew', pady=(10, 0))

    plot_all_combinations_button = ttk.Button(left_frame, text="Plot All Cut Combinations", command=plot_all_combinations)
    plot_all_combinations_button.grid(row=6, column=0, columnspan=2, sticky='ew', pady=(10, 0))

    analyze_button = ttk.Button(left_frame, text="Analyze Data on Screen", command=lambda: create_analysis_window(root))
    analyze_button.grid(row=7, column=0, columnspan=2, sticky='ew', pady=(250, 0))


    # Determine the button text based on the current plot type
    if current_plot_type == "SignalYield":
        summarize_button_text = "Summarize Yield For Specific Index"
    elif current_plot_type == "GaussianSigma":
        summarize_button_text = "Summarize Sigma For Specific Index"
    elif current_plot_type == "SBratio":
        summarize_button_text = "Summarize S/B For Specific Index"
    else:
        summarize_button_text = "Summarize Data For Specific Index"  # Default text

    summarize_button = ttk.Button(left_frame, text=summarize_button_text, command=lambda: summarize_index(root, get_history_dict_based_on_plot_type(current_plot_type)))
    summarize_button.grid(row=8, column=0, columnspan=2, sticky='ew', pady=(10, 0))

    generate_table_button = ttk.Button(
        left_frame,
        text="Generate Inv Mass Table for These Cuts",
        command=on_generate_table_click
    )
    generate_table_button.grid(row=9, column=0, columnspan=2, sticky='ew', pady=(10, 0))

    # Here we check if the current plot type is 'SignalYield'
    if current_plot_type == "SignalYield":
        transformation_button_text = "Apply √Yield/Yield Transformation"
        transformation_button = ttk.Button(left_frame, text=transformation_button_text, command=apply_transformation)
        transformation_button.grid(row=10, column=0, columnspan=2, sticky='ew', pady=(10, 0))
        centrality_frame_row = 11  # We position the centrality frame below the transformation button
        padyVal = 90
    else:
        centrality_frame_row = 10  # There is no transformation button, so centrality frame moves up
        padyVal = 100

    # Modify the LabelFrame style to center the title and underline it
    style.configure('Center.TLabelframe.Label', font=('Helvetica', 14, 'underline'), background='light gray')
    style.layout("Center.TLabelframe.Label", [('Labelframe.label', {'sticky': 'nswe'})])  # Center alignment
    centrality_frame = ttk.LabelFrame(left_frame, text="                 Hide Centrality", style='Center.TLabelframe')
    centrality_frame.grid(row=centrality_frame_row, column=0, columnspan=2, sticky="ew", pady=(padyVal, 0))



    plt.style.use('ggplot')
    # Increase the size of the plot for better focus and visibility
    fig, ax = plt.subplots(figsize=(12, 10))  # Adjust figsize to your needs


    # Create a frame for the plot to the right of the button frame
    plot_frame = ttk.Frame(root)
    plot_frame.pack(side=tk.RIGHT, fill=tk.BOTH, expand=True)
    canvas = FigureCanvasTkAgg(fig, master=plot_frame)
    canvas.get_tk_widget().pack(side=tk.TOP, fill=tk.BOTH, expand=True)



    # Pack the buttons vertically in the bottom left frame
    exit_button = ttk.Button(bottom_left_frame, text="Exit Program", command=exit_program)
    exit_button.pack(side=tk.TOP, fill=tk.X)


    save_button = ttk.Button(bottom_left_frame, text="Save Plot", command=save_plot)
    save_button.pack(side=tk.TOP, fill=tk.X)


    # Annotation setup for interactive elements on the plot.
    annot = ax.annotate('', xy=(0, 0), xytext=(20, 20), textcoords="offset points",
                        bbox=dict(boxstyle="round", fc="w"), arrowprops=dict(arrowstyle="->"))
    annot.set_visible(False)
    
    # Connect a function to handle click events on the plot.
    fig.canvas.mpl_connect('pick_event', lambda event: on_click(event, root))
    

    # Creating toggle buttons for each centrality category and placing them in centrality_frame vertically.
    centrality_labels = ["40-60%", "20-40%"]
    for i, label in enumerate(centrality_labels):
        check_var = tk.BooleanVar(value=True)  # Initialize variable to True
        # Place each checkbutton in a new row, in the first column of the centrality_frame.
        check_button = ttk.Checkbutton(centrality_frame, text=label, variable=check_var,
                                       command=lambda l=label: toggle_centrality(l))
        check_button.var = check_var  # Store the variable within the button for easy access
        check_button.grid(row=i, column=0, sticky='w', padx=10, pady=2)  # Use pady for spacing between buttons


    
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
        for container in errorbar_containers_SBratio:
            if container[0].get_gid():
                point_id = container[0].get_gid()
                centrality = plot_SBratio_history.get(point_id, {}).get('centrality')
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
                

    # Final draw call for the canvas.
    canvas.draw()

    # Apply padding around the canvas to give some space
    canvas.get_tk_widget().configure(background='light gray', highlightthickness=0)
    canvas.get_tk_widget().pack(side=tk.TOP, fill=tk.BOTH, expand=True, padx=10, pady=10)



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
    ttk.Button(initial_window, text="Signal to Background Ratio", command=lambda: set_plot_type_and_continue("SBratio")).pack()
    ttk.Button(initial_window, text="Analyze All CSV Data", command=lambda: create_csv_analysis_window(root, data)).pack()


def create_csv_analysis_window(root, csv_data):
    csv_analysis_window = tk.Toplevel(root)
    csv_analysis_window.title("CSV Data Analysis (Output to Terminal)")

    # Button for sorting the entire CSV dataset by Signal Yield
    ttk.Button(csv_analysis_window, text="Sort Entire Dataset by Signal Yield", command=lambda: sort_csv_by_signal_yield(csv_data)).pack()

    ttk.Button(csv_analysis_window, text="Sort Dataset by Yield and S/B", command=lambda: sort_csv_by_yield_and_sb(csv_data)).pack()
    
    ttk.Button(csv_analysis_window, text="Summarize CSV Data", command=lambda: summarize_csv_data(root, csv_data)).pack()


def sort_csv_by_signal_yield(csv_data):
    # Read the data from the CSV file
    sorted_data = csv_data.sort_values(by='Yield', ascending=False)

    # Loop through the centrality categories to print sorted results
    for centrality, indices in centrality_categories.items():
        # Print centrality label
        print(f"\n{centrality}:")
        print("-" * 50)  # Separator for readability

        # Sort the data for the current centrality
        centrality_data = sorted_data[sorted_data['Index'].isin(indices)]
        centrality_data_sorted = centrality_data.sort_values(by=['Index', 'Yield'], ascending=[True, False])

        # Track the last index to separate sections
        last_index = -1
        for _, row in centrality_data_sorted.iterrows():
            if last_index != row['Index']:
                last_index = row['Index']
                if last_index != -1:
                    # Print a separator between different indices
                    print()

            # Construct strings for each line
            index_label = f"Index {int(row['Index'])}: "
            yield_str = f"Yield: {row['Yield']}, "
            cuts_str = f"Cuts: E: {row['Energy']}, A: {row['Asymmetry']}, C: {row['Chi2']}, D: {row['DeltaR']}"

            # Print the information
            print(f"{index_label}{yield_str}{cuts_str}")

        # Print a separator after each centrality section
        print("-" * 50)


def sort_csv_by_yield_and_sb(csv_data):
    # Read the data from the CSV file
    sorted_data = csv_data.sort_values(by=['Yield', 'S/B'], ascending=[False, False])

    # Loop through the centrality categories to print sorted results
    for centrality, indices in centrality_categories.items():
        # Print centrality label
        print(f"\n{centrality}:")
        print("-" * 50)  # Separator for readability

        # Sort the data for the current centrality
        centrality_data = sorted_data[sorted_data['Index'].isin(indices)]
        centrality_data_sorted = centrality_data.sort_values(by=['Index', 'Yield', 'S/B'], ascending=[True, False, False])

        last_index = -1
        for _, row in centrality_data_sorted.iterrows():
            if last_index != row['Index']:
                last_index = row['Index']
                if last_index != -1:
                    print()

            # Construct strings for each line
            index_label = f"Index {int(row['Index'])}: "
            yield_sb_str = f"Yield: {row['Yield']}, S/B: {row['S/B']}, "
            cuts_str = f"Cuts: E: {row['Energy']}, A: {row['Asymmetry']}, C: {row['Chi2']}, D: {row['DeltaR']}"

            # Print the information
            print(f"{index_label}{yield_sb_str}{cuts_str}")

        print("-" * 50)  # Separator for each centrality section

    
def summarize_csv_data(root, csv_data):
    # Create a new window to display summary results
    summary_window = tk.Toplevel(root)
    summary_window.title("CSV Data Summary")

    # Create a text widget to display the summary with specified width and height
    summary_display = tk.Text(summary_window, width=100, height=25)
    summary_display.pack(expand=True, fill='both')

    # Calculate the amount of unique cut variations analyzed
    cut_columns = ['Energy', 'Asymmetry', 'Chi2', 'DeltaR']
    cut_variations = csv_data[cut_columns].drop_duplicates()
    unique_cut_combinations_count = len(cut_variations)

    # Amount of cut variations analyzed
    top_summary_text = f"Amount of cut variations analyzed: {unique_cut_combinations_count}\n\n"

    # Analysis for Centrality Categories
    summary_text = top_summary_text + "Centrality Analysis:\n"
    summary_text += analyze_categories(csv_data, centrality_categories) + "\n"

    # Analysis for pT Categories
    summary_text += "pT Analysis:\n"
    summary_text += analyze_categories(csv_data, pt_categories) + "\n"

    # Print the full summary to the terminal
    print(summary_text)

    # Insert the full summary text into the text widget
    summary_display.insert(tk.END, summary_text)
    
    # Additional analysis to find the highest yield value, cuts, and S/B for each index
    highest_yield_text = "Highest Yield Analysis by Index:\n"
    for index in range(csv_data['Index'].max() + 1):
        index_data = csv_data[csv_data['Index'] == index]
        if not index_data.empty:
            max_yield_row = index_data.loc[index_data['Yield'].idxmax()]
            highest_yield_text += f"Index {index}: Highest Yield: {max_yield_row['Yield']}, Cuts: E: {max_yield_row['Energy']}, A: {max_yield_row['Asymmetry']}, C: {max_yield_row['Chi2']}, D: {max_yield_row['DeltaR']}, S/B: {max_yield_row['S/B']}\n"

    # Print the highest yield analysis to the terminal
    highest_yield_text += "\n"  # Added newline for spacing after the highest yield analysis
    print(highest_yield_text)

    # Insert the highest yield analysis text into the text widget after the previous analyses
    summary_display.insert(tk.END, highest_yield_text)
    summary_display.see(tk.END)  # Ensure the widget scrolls to show the new analysis

    # Reset the view to the top of the text widget
    summary_display.see('1.0')


# The data is first divided based on centrality/pT bins to ensure analysis specific to each centrality/pT range
# The groupby method in Pandas is used to group the data by the unique combinations of cut values (Energy, Asymmetry, Chi2, DeltaR).
# Instead of looking at the cuts independently, this approach recognizes that the combination of cuts works together to influence the yield.
# For each group of cut combinations, the average yield is calculated.
# After calculating the average yields, the method identifies the group (or the set of cut values) with the highest average yield for each centrality/Pt category
# Traverse the data and calculate the summaries
def analyze_categories(csv_data, categories):
    analysis_text = ""
    for category, indices in categories.items():
        category_data = csv_data[csv_data['Index'].isin(indices)]

        # Group by cut combinations
        grouped = category_data.groupby(['Energy', 'Asymmetry', 'Chi2', 'DeltaR'])

        # Calculate the average yield for each group
        avg_yield = grouped['Yield'].mean()

        # Find the group with the highest average yield
        highest_avg_yield_group = avg_yield.idxmax()

        # Format the cut values
        cuts_str = f"Cuts: E: {highest_avg_yield_group[0]}, A: {highest_avg_yield_group[1]}, C: {highest_avg_yield_group[2]}, D: {highest_avg_yield_group[3]}"

        analysis_text += f"For {category}, {cuts_str} give the highest yield values on average\n\n"  # Added newline for spacing between categories

    return analysis_text

    
# Function to create the side analysis window WITHIN unique plotting windows
def create_analysis_window(root):
    analysis_window = tk.Toplevel(root)
    analysis_window.title("Further Quantitative Analysis")


    if current_plot_type == "SignalYield":
        # Button for sorting Signal Yield data
        ttk.Button(analysis_window, text="Sort by Yield with NumEntries and S/B", command=lambda: sort_by_yield_sb_and_entries(root)).pack()
    elif current_plot_type == "GaussianMean":
        # Button for sorting Gaussian Mean data
        ttk.Button(analysis_window, text="Sort by Distance from π⁰ mass", command=lambda: sort_by_gaussian_mean(root)).pack()
    elif current_plot_type == "GaussianSigma":
        ttk.Button(analysis_window, text="Sort by Gauss Sigma", command=lambda: sort_by_gaussianSigma(root)).pack()
        ttk.Button(analysis_window, text="Sort by Gauss Sigma, S/B, nEntries", command=lambda: sort_by_gaussianSigma_andSB_andNentries(root)).pack()
    elif current_plot_type == "SBratio":
        # Button for sorting Signal Yield data
        ttk.Button(analysis_window, text="Sort by S/B ratio", command=lambda: sort_by_SBratio(root)).pack()



def sort_by_yield_sb_and_entries(root):
    # Filter out the data points that are not currently visible
    visible_data = {point_id: data for point_id, data in plot_SignalYield_history.items()
                    if centrality_visibility[data['centrality']]}

    # Group data by centrality
    grouped_data = {}
    all_cuts = set()  # This set will keep track of all unique cuts combinations
    for data in visible_data.values():
        centrality_group = data['centrality']
        all_cuts.add(tuple(sorted(data['cuts'].items())))  # Add the sorted cuts to the set
        if centrality_group not in grouped_data:
            grouped_data[centrality_group] = []
        grouped_data[centrality_group].append(data)

    # Within each centrality, group by 'csv_index', then sort each group by 'y_val' in descending order
    sorted_data = {}
    highest_yield_cuts = {}  # dictionary to keep track of the highest yield cuts
    for centrality, data_list in grouped_data.items():
        # Group by 'csv_index'
        index_grouped_data = {}
        for data in data_list:
            index = data['csv_index']
            if index not in index_grouped_data:
                index_grouped_data[index] = []
            index_grouped_data[index].append(data)

        # Sort each index group by 'y_val' in descending order
        for index in index_grouped_data:
            index_grouped_data[index].sort(key=lambda x: x['y_val'], reverse=True)
            highest_yield_data = index_grouped_data[index][0]  # Since it's sorted, the first item has the highest yield
            highest_yield_cuts[index] = tuple(sorted(highest_yield_data['cuts'].items()))

        
        # Flatten the sorted groups back into a list
        sorted_data[centrality] = [item for sublist in index_grouped_data.values() for item in sublist]

    # Determine the cut values that most frequently result in the highest yield
    cuts_frequency = {}
    for cuts in highest_yield_cuts.values():
        cuts_frequency[cuts] = cuts_frequency.get(cuts, 0) + 1
    most_common_cuts = max(cuts_frequency, key=cuts_frequency.get)
    
    total_cuts_analyzed = len(all_cuts)

    # Create a new window to display sorted results
    results_window = tk.Toplevel(root)
    results_window.title("Sorted Signal Yields by Yield, S/B and NumEntries")

    # Create a text widget to display the sorted results
    results_display = tk.Text(results_window, width=100, height=25)
    results_display.pack(expand=True, fill='both')

    # Define tags for styling
    results_display.tag_configure('centrality', foreground='red')
    results_display.tag_configure('index', font=('Helvetica', '10', 'bold'))
    
    # Display the most common cuts at the top of the text widget
    most_common_cuts_str = ", ".join(f"{k}: {v}" for k, v in most_common_cuts)
    info_text = f"Most common cuts for highest yield out of {total_cuts_analyzed} total cuts analyzed: {most_common_cuts_str}\n\n"
    results_display.insert(tk.END, info_text, 'centrality')



    # Insert sorted data into the text widget, grouped by centrality and then by index
    for centrality, data_list in sorted_data.items():
        results_display.insert(tk.END, f"{centrality}:\n", 'centrality')
        last_index = None
        for data in data_list:
            if last_index != data['csv_index']:
                if last_index is not None:
                    results_display.insert(tk.END, "\n")  # Add a newline for new index groups
                last_index = data['csv_index']
            cuts_str = ", ".join(f"{k}: {v}" for k, v in data['cuts'].items())
            results_display.insert(tk.END, f"Index {data['csv_index']}: ", 'index')
            # Display yield with associated error
            results_display.insert(tk.END, f"Yield: {data['y_val']} \u00B1 {data['y_err']}, S/B: {data['S_B_ratio']}, NumEntry: {data['numEntry']}, Cuts: {cuts_str}\n")
        results_display.insert(tk.END, "\n\n")  # Add extra newline after each centrality group for better readability

    # Scroll back to the top of the text widget
    results_display.see('1.0')


def sort_by_gaussianSigma(root):
    # Filter out the data points that are not currently visible
    visible_data = {point_id: data for point_id, data in plot_GaussianSigma_history.items()
                    if centrality_visibility[data['centrality']]}

    # Group data by centrality
    grouped_data = {}
    for data in visible_data.values():
        centrality_group = data['centrality']
        if centrality_group not in grouped_data:
            grouped_data[centrality_group] = []
        grouped_data[centrality_group].append(data)
    
    # Within each centrality, group by 'csv_index', then sort each group by 'y_val' in descending order
    sorted_data = {}
    for centrality, data_list in grouped_data.items():
        # Group by 'csv_index'
        index_grouped_data = {}
        for data in data_list:
            index = data['csv_index']
            if index not in index_grouped_data:
                index_grouped_data[index] = []
            index_grouped_data[index].append(data)

        # Sort each index group by 'y_val' in descending order
        for index in index_grouped_data:
            index_grouped_data[index].sort(key=lambda x: x['sigma_val'], reverse=True)
        
        # Flatten the sorted groups back into a list
        sorted_data[centrality] = [item for sublist in index_grouped_data.values() for item in sublist]


    results_window = tk.Toplevel(root)
    results_window.title("Sorted Gauss Sigma")
    results_window.geometry('600x400')  # Set a default size for the window

    # Create a scrollbar widget and pack it
    scrollbar = tk.Scrollbar(results_window)
    scrollbar.pack(side=tk.RIGHT, fill=tk.Y)

    # Create a text widget and pack it with fill and expand options
    results_display = tk.Text(results_window, yscrollcommand=scrollbar.set)
    results_display.pack(fill='both', expand=True)  # This allows the text box to resize

    # Configure the scrollbar to scroll the text widget
    scrollbar.config(command=results_display.yview)

    # Define tags for styling
    results_display.tag_configure('centrality', foreground='red')
    results_display.tag_configure('index', font=('Helvetica', '10', 'bold'))

    # Insert sorted data into the text widget, grouped by centrality and then by index
    for centrality, data_list in sorted_data.items():
        results_display.insert(tk.END, f"{centrality}:\n", 'centrality')
        last_index = None
        for data in data_list:
            if last_index != data['csv_index']:
                if last_index is not None:
                    results_display.insert(tk.END, "\n")  # Add a newline for new index groups
                last_index = data['csv_index']
            cuts_str = ", ".join(f"{k}: {v}" for k, v in data['cuts'].items())
            results_display.insert(tk.END, f"Index {data['csv_index']}: ", 'index')
            results_display.insert(tk.END, f"Sigma: {data['sigma_val']}, Cuts: {cuts_str}\n")
        results_display.insert(tk.END, "\n\n")  # Add extra newline after each centrality group for better readability


    # Scroll back to the top of the text widget
    results_display.see('1.0')

def sort_by_gaussianSigma_andSB_andNentries(root):
    # Filter out the data points that are not currently visible
    visible_data = {point_id: data for point_id, data in plot_GaussianSigma_history.items()
                    if centrality_visibility[data['centrality']]}

    # Group data by centrality
    grouped_data = {}
    for data in visible_data.values():
        centrality_group = data['centrality']
        if centrality_group not in grouped_data:
            grouped_data[centrality_group] = []
        grouped_data[centrality_group].append(data)

    # Within each centrality, group by 'csv_index'
    sorted_data = {}
    for centrality, data_list in grouped_data.items():
        # Group by 'csv_index'
        index_grouped_data = {}
        for data in data_list:
            index = data['csv_index']
            if index not in index_grouped_data:
                index_grouped_data[index] = []
            index_grouped_data[index].append(data)

        # Sort each index group by 'sigma_val', then 'S_B_ratio', and then 'numEntry' in descending order
        for index in index_grouped_data:
            index_grouped_data[index].sort(key=lambda x: (x['sigma_val'], x['S_B_ratio'], x['numEntry']), reverse=True)
        
        # Flatten the sorted groups back into a list
        sorted_data[centrality] = [item for sublist in index_grouped_data.values() for item in sublist]

    # Create a new window to display sorted results
    results_window = tk.Toplevel(root)
    results_window.title("Sorted Gauss Sigma by Sigma, S/B, NumEntries, and Sigma Error")

    # Create a text widget to display the sorted results
    results_display = tk.Text(results_window, width=100, height=25)
    results_display.pack(expand=True, fill='both')

    # Define tags for styling
    results_display.tag_configure('centrality', foreground='red')
    results_display.tag_configure('index', font=('Helvetica', '10', 'bold'))

    # Insert sorted data into the text widget, grouped by centrality and then by index
    for centrality, data_list in sorted_data.items():
        results_display.insert(tk.END, f"{centrality}:\n", 'centrality')
        last_index = None
        for data in data_list:
            if last_index != data['csv_index']:
                if last_index is not None:
                    results_display.insert(tk.END, "\n")  # Add a newline for new index groups
                last_index = data['csv_index']
            cuts_str = ", ".join(f"{k}: {v}" for k, v in data['cuts'].items())
            results_display.insert(tk.END, f"Index {data['csv_index']}: ", 'index')
            results_display.insert(tk.END, f"Sigma: {data['sigma_val']} \u00B1 {data['sigma_err']}, S/B: {data['S_B_ratio']}, NumEntry: {data['numEntry']}, Cuts: {cuts_str}\n")
        results_display.insert(tk.END, "\n\n")  # Add extra newline after each centrality group for better readability

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
    results_display = tk.Text(results_window, width = 100, height = 25)
    results_display.pack(expand = True, fill = 'both')

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
    
    

# Function to perform the sorting of signal yield and display results
def sort_by_SBratio(root):
    # Filter out the data points that are not currently visible
    visible_data = {point_id: data for point_id, data in plot_SBratio_history.items()
                    if centrality_visibility[data['centrality']]}

    # Group data by centrality
    grouped_data = {}
    for data in visible_data.values():
        centrality_group = data['centrality']
        if centrality_group not in grouped_data:
            grouped_data[centrality_group] = []
        grouped_data[centrality_group].append(data)
    
    # Within each centrality, group by 'csv_index', then sort each group by 'y_val' in descending order
    sorted_data = {}
    for centrality, data_list in grouped_data.items():
        # Group by 'csv_index'
        index_grouped_data = {}
        for data in data_list:
            index = data['csv_index']
            if index not in index_grouped_data:
                index_grouped_data[index] = []
            index_grouped_data[index].append(data)

        # Sort each index group by 'y_val' in descending order
        for index in index_grouped_data:
            index_grouped_data[index].sort(key=lambda x: x['sb_val'], reverse=True)
        
        # Flatten the sorted groups back into a list
        sorted_data[centrality] = [item for sublist in index_grouped_data.values() for item in sublist]


    results_window = tk.Toplevel(root)
    results_window.title("Sorted SBratio")
    results_window.geometry('600x400')  # Set a default size for the window

    # Create a scrollbar widget and pack it
    scrollbar = tk.Scrollbar(results_window)
    scrollbar.pack(side=tk.RIGHT, fill=tk.Y)

    # Create a text widget and pack it with fill and expand options
    results_display = tk.Text(results_window, yscrollcommand=scrollbar.set)
    results_display.pack(fill='both', expand=True)  # This allows the text box to resize

    # Configure the scrollbar to scroll the text widget
    scrollbar.config(command=results_display.yview)

    # Define tags for styling
    results_display.tag_configure('centrality', foreground='red')
    results_display.tag_configure('index', font=('Helvetica', '10', 'bold'))

    # Insert sorted data into the text widget, grouped by centrality and then by index
    for centrality, data_list in sorted_data.items():
        results_display.insert(tk.END, f"{centrality}:\n", 'centrality')
        last_index = None
        for data in data_list:
            if last_index != data['csv_index']:
                if last_index is not None:
                    results_display.insert(tk.END, "\n")  # Add a newline for new index groups
                last_index = data['csv_index']
            cuts_str = ", ".join(f"{k}: {v}" for k, v in data['cuts'].items())
            results_display.insert(tk.END, f"Index {data['csv_index']}: ", 'index')
            results_display.insert(tk.END, f"S/B: {data['sb_val']}, Cuts: {cuts_str}\n")
        results_display.insert(tk.END, "\n\n")  # Add extra newline after each centrality group for better readability


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
