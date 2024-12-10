import numpy as np
import matplotlib.pyplot as plt
from matplotlib.colors import ListedColormap

# Get the original viridis colormap
original_cmap = plt.get_cmap('viridis')

# Convert the colormap to a list of RGBA colors
N = 256  # Number of colors in the colormap
colors = original_cmap(np.linspace(0, 1, N))

# Replace the violet section with white
# Violet corresponds roughly to the lowest range of viridis
# Adjust the range as needed based on visual inspection
for i in range(N):
    if i < 50:  # Replace the first 50 colors (adjust as needed for violet range)
        colors[i] = [1.0, 1.0, 1.0, 1.0]  # RGBA for white

# Create a new colormap with the modified colors
modified_cmap = ListedColormap(colors)

# Test the new colormap
gradient = np.linspace(0, 1, 100).reshape(1, -1)
plt.imshow(gradient, aspect='auto', cmap=modified_cmap)
plt.colorbar()
plt.title('Viridis with Violet Replaced by White')
plt.show()

# Transform numpy grid into dense matrix
sparse_grid_OFP = csr_matrix(grid_OFP_np)

# Vérifier la taille en mémoire --> dense matrix
print(f"Taille dense : {grid_OFP_np.nbytes / 1e6:.2f} MB")
print(f"Taille sparse : {sparse_grid_OFP.data.nbytes / 1e6:.2f} MB")

# Reconvertir en dense pour l'affichage
dense_grid = sparse_grid_OFP.toarray()
plt.figure(3)
plt.imshow(dense_grid, origin='lower', cmap='YlOrRd')
plt.title("OFP - grid based on real coordinates")#gives us very low values
plt.xlabel("x index")
plt.ylabel("y index")
plt.colorbar()
plt.savefig('Results/OFP_based_on_real_coordinates_dense_matrix.png')


#-----Calculate total amounts------------------------
OFP_tot = np.sum(grid_OFP_np)
PM10_tot = np.sum(grid_PM10_np)
O3_tot = np.sum(grid_O3_np)
summary.write(f"Total amount of ozone emitted: {OFP_tot} kg/y \n ")
summary.write(f"Total amount of PM10 absorbed: {PM10_tot} mikrog/y \n")
summary.write(f"Total amount of ozone absorbed: {O3_tot} kg/y\n")