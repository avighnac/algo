import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d.art3d import Poly3DCollection

# =====================================================
# 1. EDIT / GENERATE YOUR 3D POINTS HERE
#    (0, 0, 0) is the CENTER CUBE.
#    (1, 0, 0) is one cube right,
#    (0, 1, 0) is up,
#    (0, 0, 1) is forward.
# =====================================================

points = set()

d = 2
for i in range(-d, d+1):
    for j in range(-d, d+1):
        for k in range(-d, d+1):
            if abs(i) + abs(j) + abs(k) <= d:
                # Your transformation, if you want one:
                # points.add((i+j, j-i, k))
                points.add((j+i, j-i, k))  # raw diamond

# -----------------------------------------------------
# Below this line nothing needs editing
# -----------------------------------------------------

if not points:
    print("No cubes to draw.")
    raise SystemExit


def cube_vertices(x, y, z, size=1):
    """Return the 8 vertices of a cube centered at (x,y,z)."""
    s = size / 2
    return [
        (x - s, y - s, z - s),
        (x + s, y - s, z - s),
        (x + s, y + s, z - s),
        (x - s, y + s, z - s),
        (x - s, y - s, z + s),
        (x + s, y - s, z + s),
        (x + s, y + s, z + s),
        (x - s, y + s, z + s),
    ]


xs = [x for x, _, _ in points]
ys = [y for _, y, _ in points]
zs = [z for _, _, z in points]

min_x, max_x = min(xs), max(xs)
min_y, max_y = min(ys), max(ys)
min_z, max_z = min(zs), max(zs)

padding = 1

fig = plt.figure()
ax = fig.add_subplot(111, projection="3d")

# Draw faint cube grid
for gx in range(min_x - padding, max_x + padding + 1):
    for gy in range(min_y - padding, max_y + padding + 1):
        for gz in range(min_z - padding, max_z + padding + 1):
            pass  # gridlines can be added, but they clutter 3D view

# Draw cubes
for (x, y, z) in points:
    v = cube_vertices(x, y, z)
    faces = [
        [v[0], v[1], v[2], v[3]],
        [v[4], v[5], v[6], v[7]],
        [v[0], v[1], v[5], v[4]],
        [v[2], v[3], v[7], v[6]],
        [v[1], v[2], v[6], v[5]],
        [v[4], v[7], v[3], v[0]],
    ]
    cube = Poly3DCollection(
        faces,
        alpha=0.6,
        facecolor="gray",
        edgecolor="black",
        linewidths=0.5
    )
    ax.add_collection3d(cube)

# Set limits
ax.set_xlim(min_x - padding, max_x + padding)
ax.set_ylim(min_y - padding, max_y + padding)
ax.set_zlim(min_z - padding, max_z + padding)

ax.set_box_aspect([1, 1, 1])  # equal aspect ratio

ax.set_xlabel("x")
ax.set_ylabel("y")
ax.set_title("3D Grid of Cubes (drag to rotate!)")

plt.show()
