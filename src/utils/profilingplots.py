import pandas as pd
import matplotlib.pyplot as plt
import glob
import os

# Find the latest statistics file
list_of_files = glob.glob('statistics/flow_profile_it*.csv')
if not list_of_files:
    print("No CSV files found in build/statistics/")
else:
    for file in list_of_files:
        print(f"Plotting results from: {file}")

        # Load data
        df = pd.read_csv(file)

        fig, ax1 = plt.subplots(figsize=(10, 6))

        # Plot Velocity (The Parabola)
        color = 'tab:blue'
        ax1.set_xlabel('Position along X-axis (Channel Width)')
        ax1.set_ylabel('Average Velocity Vy', color=color)
        ax1.plot(df['BinMidPoint'], df['AverageVelocityY'], marker='o', color=color, label='Velocity')
        ax1.tick_params(axis='y', labelcolor=color)
        ax1.grid(True, linestyle='--', alpha=0.6)

        # Plot Density (The Layering)
        ax2 = ax1.twinx()
        color = 'tab:red'
        ax2.set_ylabel('Particle Count (Density)', color=color)
        ax2.plot(df['BinMidPoint'], df['Density'], marker='x', color=color, alpha=0.5, label='Density')
        ax2.tick_params(axis='y', labelcolor=color)

        plt.title(f'Flow Profile at {file}')
        fig.tight_layout()