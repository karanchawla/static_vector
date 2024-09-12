import matplotlib.pyplot as plt
import numpy as np

# Data from benchmark results
sizes = [8, 64, 512, 4096, 8192]
operations = ['Construction', 'PushBack', 'EmplaceBack', 'RandomAccess', 'Iteration', 'PopBack']

std_vector_times = {
    'Construction': [40.7, 80.5, 275, 1933, 4173],
    'PushBack': [10.3, 28.3, 199, 1500, 3640],
    'EmplaceBack': [9.92, 26.5, 192, 1432, 3442],
    'RandomAccess': [3.14, 22.2, 210, 1023, 2170],
    'Iteration': [1.23, 13.9, 68.4, 489, 971],
    'PopBack': [111, 145, 219, 736, 1317]
}

static_vector_times = {
    'Construction': [2.82, 34.6, 225, 1836, 4336],
    'PushBack': [2.92, 38.4, 237, 1799, 4123],
    'EmplaceBack': [2.83, 27.5, 225, 1777, 4106],
    'RandomAccess': [1.91, 12.6, 96.7, 1102, 2137],
    'Iteration': [1.42, 11.8, 69.5, 491, 973],
    'PopBack': [129, 147, 242, 993, 1860]
}

plt.figure(figsize=(20, 15))
plt.style.use('seaborn')

for i, operation in enumerate(operations, 1):
    plt.subplot(3, 2, i)
    
    plt.plot(sizes, std_vector_times[operation], 'o-', label='std::vector', linewidth=2, markersize=8)
    plt.plot(sizes, static_vector_times[operation], 's-', label='static_vector', linewidth=2, markersize=8)
    
    plt.title(f'{operation} Performance', fontsize=16)
    plt.xlabel('Number of Elements', fontsize=12)
    plt.ylabel('Time (ns)', fontsize=12)
    plt.xscale('log')
    plt.yscale('log')
    plt.legend(fontsize=10)
    plt.grid(True, which="both", ls="-", alpha=0.2)
    
    # Add value labels
    for x, y in zip(sizes, std_vector_times[operation]):
        plt.annotate(f'{y}', (x,y), textcoords="offset points", xytext=(0,10), ha='center', fontsize=8)
    for x, y in zip(sizes, static_vector_times[operation]):
        plt.annotate(f'{y}', (x,y), textcoords="offset points", xytext=(0,-10), ha='center', fontsize=8)

plt.tight_layout()
plt.suptitle('Performance Comparison: std::vector vs static_vector', fontsize=20)
plt.subplots_adjust(top=0.93)

plt.savefig('vector_performance_comparison.png', dpi=300, bbox_inches='tight')
plt.show()