import numpy as np
from scipy.optimize import curve_fit

# Experimental data from the paper
data = '''
0.9897674418604651, 0.028205128205128216
0.8332558139534884, 0.3807692307692308
0.9613953488372092, 0.15000000000000002
0.979767441860465, 0.0641025641025641
0.9432558139534883, 0.22051282051282056
0.925813953488372, 0.27051282051282055
0.9090697674418604, 0.2935897435897436
0.8886046511627906, 0.34102564102564104
0.8695348837209302, 0.35000000000000003
0.7995348837209302, 0.4012820512820513
0.7688372093023256, 0.45
'''

# Parse data
lines = [l.strip() for l in data.strip().split('\n') if l.strip()]
x_data = []  # a2/a1
y_data = []  # 3U * a2/a1

for line in lines:
    parts = line.split(',')
    x_data.append(float(parts[0]))
    y_data.append(float(parts[1]))

x_data = np.array(x_data)
y_data = np.array(y_data)

# Sort by x
idx = np.argsort(x_data)
x_data = x_data[idx]
y_data = y_data[idx]

# Compute U from the scaled velocity
# y = 3U * x  =>  U = y / (3 * x)
U_data = y_data / (3.0 * x_data)

print('Experimental Data Analysis:')
print('=' * 60)
print(f'{'a2/a1':<12} {'3U*a2/a1':<15} {'U (derived)':<12}')
print('-' * 60)
for x, y, u in zip(x_data, y_data, U_data):
    print(f'{x:<12.4f} {y:<15.4f} {u:<12.4f}')

# Now fit U as a function of deflation (1 - a2/a1)
deflation = 1.0 - x_data

# Try different models
print('\n' + '=' * 60)
print('Fitting Models:')
print('=' * 60)

# Model 1: Power law U = A * (1-nu)^B
def power_law(d, A, B):
    return A * np.power(d, B)

popt1, _ = curve_fit(power_law, deflation, U_data, p0=[1.0, 0.8], bounds=([0, 0], [10, 3]))
print(f'\nModel 1: U = A * (1-ν)^B')
print(f'  A = {popt1[0]:.4f}, B = {popt1[1]:.4f}')
U_pred1 = power_law(deflation, *popt1)
rmse1 = np.sqrt(np.mean((U_data - U_pred1)**2))
print(f'  RMSE = {rmse1:.6f}')

# Model 2: Polynomial U = a0 + a1*(1-nu) + a2*(1-nu)^2
def poly2(d, a0, a1, a2):
    return a0 + a1*d + a2*d**2

popt2, _ = curve_fit(poly2, deflation, U_data)
print(f'\nModel 2: U = a0 + a1*(1-ν) + a2*(1-ν)^2')
print(f'  a0 = {popt2[0]:.4f}, a1 = {popt2[1]:.4f}, a2 = {popt2[2]:.4f}')
U_pred2 = poly2(deflation, *popt2)
rmse2 = np.sqrt(np.mean((U_data - U_pred2)**2))
print(f'  RMSE = {rmse2:.6f}')

# Model 3: Linear through origin U = A * (1-nu)
def linear_origin(d, A):
    return A * d

popt3, _ = curve_fit(linear_origin, deflation, U_data)
print(f'\nModel 3: U = A * (1-ν)')
print(f'  A = {popt3[0]:.4f}')
U_pred3 = linear_origin(deflation, *popt3)
rmse3 = np.sqrt(np.mean((U_data - U_pred3)**2))
print(f'  RMSE = {rmse3:.6f}')

# Model 4: Cubic U = a1*(1-nu) + a2*(1-nu)^2 + a3*(1-nu)^3 (forced through origin)
def poly3_origin(d, a1, a2, a3):
    return a1*d + a2*d**2 + a3*d**3

popt4, _ = curve_fit(poly3_origin, deflation, U_data)
print(f'\nModel 4: U = a1*(1-ν) + a2*(1-ν)^2 + a3*(1-ν)^3')
print(f'  a1 = {popt4[0]:.4f}, a2 = {popt4[1]:.4f}, a3 = {popt4[2]:.4f}')
U_pred4 = poly3_origin(deflation, *popt4)
rmse4 = np.sqrt(np.mean((U_data - U_pred4)**2))
print(f'  RMSE = {rmse4:.6f}')

# Model 5: Quadratic through origin U = a1*(1-nu) + a2*(1-nu)^2
def poly2_origin(d, a1, a2):
    return a1*d + a2*d**2

popt5, _ = curve_fit(poly2_origin, deflation, U_data)
print(f'\nModel 5: U = a1*(1-ν) + a2*(1-ν)^2')
print(f'  a1 = {popt5[0]:.4f}, a2 = {popt5[1]:.4f}')
U_pred5 = poly2_origin(deflation, *popt5)
rmse5 = np.sqrt(np.mean((U_data - U_pred5)**2))
print(f'  RMSE = {rmse5:.6f}')

print('\n' + '=' * 60)
print('Best Model Selection:')
print('=' * 60)
models = [('Power law', rmse1), ('Polynomial', rmse2), ('Linear', rmse3), 
          ('Cubic origin', rmse4), ('Quadratic origin', rmse5)]
models.sort(key=lambda x: x[1])
for name, rmse in models:
    print(f'  {name}: RMSE = {rmse:.6f}')

print('\n' + '=' * 60)
print('Recommended: Model 5 (Quadratic through origin)')
print(f'U(ν) = {popt5[0]:.4f}*(1-ν) + {popt5[1]:.4f}*(1-ν)^2')
print('=' * 60)
