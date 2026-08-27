import pandas as pd
import matplotlib.pyplot as plt

fitness_df = pd.read_csv('C:\\Users\\alber\\Desktop\\shooters\\fitness.csv', header=None,
    names=['best_fitness', 'avg_fitness', 'worst_fitness', 'median', 'q1', 'q3', 'std_dev'])

actions_df = pd.read_csv('C:\\Users\\alber\\Desktop\\shooters\\actions_count.csv', header=None,
                          names=['shoot', 'move_up', 'move_down', 'rotate_left', 'rotate_right'])

print(fitness_df.head(10))
print(fitness_df['std_dev'].describe())
print(fitness_df['median'].describe())

epochs = range(1, len(fitness_df) + 1)
avg = fitness_df['avg_fitness']
std = fitness_df['std_dev'].abs()

fig, axes = plt.subplots(4, 1, figsize=(12, 18))

# --- Panel 1: best + Q3 ---
axes[0].plot(epochs, fitness_df['best_fitness'], color='green', alpha=0.7, label='best')
axes[0].plot(epochs, fitness_df['q3'], color='blue', alpha=0.7, label='Q3')
axes[0].set_title('Best + Q3')
axes[0].set_xlabel('Epoch')
axes[0].set_ylabel('Fitness')
axes[0].legend()

# --- Panel 2: avg + sigma bands + median (widok rozrzutu) ---
axes[1].plot(epochs, avg, color='blue', label='avg')
axes[1].plot(epochs, fitness_df['median'], color='orange', linestyle='--', label='median')
axes[1].fill_between(epochs, avg - std, avg + std, alpha=0.3, color='blue', label='±1σ')
axes[1].fill_between(epochs, avg - 2*std, avg + 2*std, alpha=0.1, color='blue', label='±2σ')
axes[1].set_title('Average ± Sigma Bands + Median')
axes[1].set_xlabel('Epoch')
axes[1].set_ylabel('Fitness')
axes[1].legend()

# --- Panel 3: std dev ---
axes[2].plot(epochs, std, color='purple')
axes[2].set_title('Genetic Diversity (Std Dev)')
axes[2].set_xlabel('Epoch')
axes[2].set_ylabel('Std Dev')

# --- Panel 4: avg vs median (czytelna skala, skupiona na srodku) ---
axes[3].plot(epochs, avg, color='blue', label='avg')
axes[3].plot(epochs, fitness_df['median'], color='orange', linestyle='--', label='median')
lo = min(avg.min(), fitness_df['median'].min())
hi = max(avg.max(), fitness_df['median'].max())
pad = (hi - lo) * 0.2 + 1
axes[3].set_ylim(lo - pad, hi + pad)
axes[3].set_title('Average vs Median')
axes[3].set_xlabel('Epoch')
axes[3].set_ylabel('Fitness')
axes[3].legend()

plt.tight_layout()

# --- wykres 2: akcje ---
fig2, ax = plt.subplots(figsize=(7, 7))
totals = actions_df.sum()
ax.pie(totals, labels=totals.index, autopct='%1.1f%%',
       colors=['red', 'blue', 'green', 'orange', 'purple'])
ax.set_title('Action Distribution')

plt.tight_layout()
plt.show()
