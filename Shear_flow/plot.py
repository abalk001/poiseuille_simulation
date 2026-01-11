import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.image as mpimg

img = mpimg.imread("img.png")

df = pd.read_csv("plot-data.csv")
df = df.sort_values(df.columns[0])

x = df.iloc[:, 0].values
y = df.iloc[:, 1].values

xmin, xmax = 0.75, 1.00
ymin, ymax = 0.0, 0.7

fig, ax = plt.subplots(figsize=(7, 4))

ax.imshow(
    img,
    extent=[xmin, xmax, ymin, ymax],
    origin="upper",
    aspect="auto"
)

ax.scatter(x, y, s=30)
ax.plot(x, y, linewidth=2, label="data points")

ax.set_xlim(xmin, xmax)
ax.set_ylim(ymin, ymax)
plt.legend(loc="upper right")

ax.set_xlabel(r"$a_2 / \hat{a}_1$")
ax.set_ylabel(r"$3U(a_1)/\hat{\sigma}_1$")

plt.show()
fig.savefig("plot.png", dpi=300)