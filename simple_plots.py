import pandas as pd
import matplotlib.pyplot as plt

fitness_df = pd.read_csv('C:\\Users\\alber\\Desktop\\shooters\\fitness.csv', header=None,
                          usecols=[0, 1, 2],
                          names=['best_fitness', 'avg_fitness', 'worst_fitness'])
actions_df = pd.read_csv('C:\\Users\\alber\\Desktop\\shooters\\actions_count.csv', header=None,
                          names=['shoot', 'move_up', 'move_down', 'rotate_left', 'rotate_right'])

epochs_f = range(1, len(fitness_df) + 1)

# --- wykres 1: fitness ---
fig1, axes = plt.subplots(3, 1, figsize=(12, 10))

axes[0].plot(epochs_f, fitness_df['best_fitness'], color='green')
axes[0].set_title('Best Fitness')
axes[0].set_xlabel('Epoch')
axes[0].set_ylabel('Fitness')

axes[1].plot(epochs_f, fitness_df['avg_fitness'], color='blue')
axes[1].set_title('Average Fitness')
axes[1].set_xlabel('Epoch')
axes[1].set_ylabel('Fitness')

axes[2].plot(epochs_f, fitness_df['worst_fitness'], color='red')
axes[2].set_title('Worst Fitness')
axes[2].set_xlabel('Epoch')
axes[2].set_ylabel('Fitness')

plt.tight_layout()

# --- wykres 2: akcje ---
fig2, ax = plt.subplots(figsize=(7, 7))
totals = actions_df.sum()
ax.pie(totals, labels=totals.index, autopct='%1.1f%%',
       colors=['red', 'blue', 'green', 'orange', 'purple'])
ax.set_title('Action Distribution (total)')

plt.tight_layout()
plt.show()