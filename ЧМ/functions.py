import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import config
import os
import csv
from matplotlib.ticker import MaxNLocator

# print(plt.style.available)
plt.style.use('seaborn-v0_8-muted')

def Cp_function_low(T, a, b, c, d, e):
    """ Вычисление теплоемкости при низких температурах """
    return a + b * (T ** (-2)) + c * T + d * (T ** 2) + e * (T ** 3)

def Cp_function_high(T, a, b, c, d, e):
    """ Вычисление теплоемкости при высоких температурах температурах с поправкой T_check """
    T_check = config.max_low_temperature
    return a + b * ((1 / (T**2)) - (1 / (T_check ** 2))) + c * (T - T_check) + d * ((T**2) - (T_check**2)) + e * ((T**3) - (T_check**3))

def H_function_low(T, a, b, c, d , e, H_0):
    """ Вычисление энтальпия при низких температурах температурах """
    x = T
    integral_1 = a * x - b / x + c * (x**2 / 2) + d * (x**3 / 3) + e * (x**4 / 4)
    x = config.T_0
    integral_2 = a * x - b / x + c * (x**2 / 2) + d * (x**3 / 3) + e * (x**4 / 4)
    H = integral_1 - integral_2 + H_0
    return H / 1000

def H_function_high(T, a, b, c, d, e, H_0):
    """ Вычисление энтальпия при высоких температурах температурах с поправкой T_check """
    x = T
    integral_1 = a * x - b / x + c * (x**2 / 2) + d * (x**3 / 3) + e * (x**4 / 4)
    # T_check = config.max_low_temperature
    # integral_1 = a * x + b * ( -1 / x  + x / (T_check**2)) + c * (((x**2) / 2) - (x * T_check)) + d * (((x**3) / 3) - (T_check**2) * x) + e * (((x**4) / 4) - ((T_check**3) * x))
    x = config.max_low_temperature
    integral_2 = a * x - b / x + c * (x**2 / 2) + d * (x**3 / 3) + e * (x**4 / 4)
    # integral_2 = a * x + b * ( -1 / x  + x / (T_check**2)) + c * (((x**2) / 2) - (x * T_check)) + d * (((x**3) / 3) - (T_check**2) * x) + e * (((x**4) / 4) - ((T_check**3) * x))
    H = integral_1 - integral_2 + H_0
    return H / 1000

def S_function_low(T, a, b, c, d , e, S_0):
    """ Вычисление энтропии при низких температурах температурах """
    x = T
    integral_1 = a * np.log(np.abs(x)) - b / (2 * (x ** 2)) + c * (x) + d * ((x**2) / 2) + e * ((x**3) / 3)
    x = config.T_0
    integral_2 = a * np.log(np.abs(x)) - b / (2 * (x ** 2)) + c * (x) + d * ((x**2) / 2) + e * ((x**3) / 3)
    S = integral_1 - integral_2 + S_0
    return S

def S_function_high(T, a, b, c, d, e, S_0):
    """ Вычисление энтропии при высоких температурах температурах с поправкой T_check """
    x = T
    # integral_1 = a * np.log(np.abs(x)) - b / (2 * (x ** 2)) + c * (x) + d * ((x**2) / 2) + e * ((x**3) / 3)
    T_check = config.max_low_temperature
    integral_1 = a * np.log(np.abs(x)) + b * (-1 / (2 * (x**2)) - np.log(np.abs(x)) / (T_check**2)) + c * (x - np.log(np.abs(x)) * T_check) + d * ((x**2 / 2) - (T_check**2) * np.log(np.abs(x))) + e * (x**3 / 3 - (T_check**3) * np.log(np.abs(x)))
    x = config.max_low_temperature
    # integral_2 = a * np.log(np.abs(x)) - b / (2 * (x ** 2)) + c * (x) + d * ((x**2) / 2) + e * ((x**3) / 3)
    integral_2 = a * np.log(np.abs(x)) + b * (-1 / (2 * (x**2)) - np.log(np.abs(x)) / (T_check**2)) + c * (x - np.log(np.abs(x)) * T_check) + d * ((x**2 / 2) - (T_check**2) * np.log(np.abs(x))) + e * (x**3 / 3 - (T_check**3) * np.log(np.abs(x)))
    S = integral_1 - integral_2 + S_0
    return S

def least_squares_fit_cp(T, Cp):
    """ МНК для малых температур """
    T = np.array(T)
    Cp = np.array(Cp)
    X = np.vstack([            # склейка строк
        np.ones(len(T)),       # массив единиц (а)
        T**-2,                 # b / T
        T,                     # c * T
        T**2,                  # d * T^2
        T**3                   # e * T^3
    ]).T                       # транспонирование

    # Решение системы линейных уравнений: (XᵀX)a = Xᵀy
    XtX = np.dot(X.T, X) # произвеление (XᵀX)
    Xty = np.dot(X.T, Cp) # произвеление Xᵀy
    coefficients = np.linalg.solve(XtX, Xty) # решение Ах=В
    return coefficients

def least_squares_fit_cp_2(T, Cp):
    """ МНК для высоких температур """
    T = np.array(T)
    Cp = np.array(Cp)
    X = np.column_stack([ # склейка столбцов
        T**-2 - config.max_low_temperature**-2,             # b * (1 / T^2 - 1 / (T_docking)^2)
        T - config.max_low_temperature,                     # c * (T- T_docking)
        T**2 - config.max_low_temperature**2,               # d * (T^2- T_docking ^ 2)
        T**3 - config.max_low_temperature**3                # e * (T^3- T_docking ^ 3)
    ])

    # Решение системы линейных уравнений: (XᵀX)a = Xᵀy
    XtX = np.dot(X.T, X)
    Xty = np.dot(X.T, Cp)
    coefficients = np.linalg.solve(XtX, Xty)
    return coefficients

def get_real_values(df):
    """ Извлечение данных """
    temperatures = df['Temperature'].values
    cp_values = df['Cp'].values
    S_values = df['S'].values
    H_values = df['H'].values
    return temperatures, cp_values, S_values, H_values

def print_data(name, Cp_low, Cp_high):
    print(f"Данные для " + name)
    print(f"Cp при T={config.max_low_temperature} K (низкий диапазон): {Cp_low:.3f}")
    print(f"Cp при T={config.max_low_temperature} K (высокий диапазон, после стыковки): {Cp_high:.3f}")

def print_coef(name, coeff_low, coeff_high):
    print(f"Коэффициенты для " + name)
    print(f"Коэффициенты при T<{config.max_low_temperature} K (низкий диапазон): {' '.join(map(str, coeff_low))}")
    print(f"Коэффициенты при T>{config.max_low_temperature} K (высокий диапазон): {' '.join(map(str, coeff_high))}")

def get_properties(T, coeffs_low, coeffs_high, H_0, S_0):
    T_low = T[T <= config.max_low_temperature]
    T_high = T[(T >= config.max_low_temperature) & (T <= config.max_high_temperature)]
    
    Cp_fit_low = Cp_function_low(T_low, *coeffs_low)
    Cp_fit_high = Cp_function_high(T_high, *coeffs_high)
    
    H_fit_low = H_function_low(T_low, *coeffs_low, H_0)
    H_fit_high = H_function_high(T_high, *coeffs_high, H_fit_low[-1] * 1000)
    # print(f"H0={H_fit_low[-1]} * 1000")
    
    S_fit_low = S_function_low(T_low, *coeffs_low, S_0)
    S_fit_high = S_function_high(T_high, *coeffs_high, S_fit_low[-1])
    # print(f"S0={S_fit_low[-1]}")
    
    return Cp_fit_low, Cp_fit_high, H_fit_low, H_fit_high, S_fit_low, S_fit_high

def get_data(filename, name):
    df = pd.read_csv(filename)
    df['Temperature'] = pd.to_numeric(df['Temperature'], errors='coerce')
    df['Cp'] = pd.to_numeric(df['Cp'], errors='coerce')
    df['S'] = pd.to_numeric(df['S'], errors='coerce')
    df['H'] = pd.to_numeric(df['H'], errors='coerce')
    df = df.dropna()
    
    temperatures, cp_values, S_values, H_values = get_real_values(df)
    
    T_low = temperatures[temperatures <= config.max_low_temperature]
    Cp_low = cp_values[temperatures <= config.max_low_temperature]
    
    T_high = temperatures[(temperatures <= config.max_high_temperature) &
                          (temperatures >= config.max_low_temperature)]
    Cp_high = cp_values[(temperatures <= config.max_high_temperature) &
                        (temperatures >= config.max_low_temperature)]
    
    coeff_low = least_squares_fit_cp(T_low, Cp_low)
    
    T_check = config.max_low_temperature
    a_high = Cp_function_low(T_check, *coeff_low)
    coeff_high_temperatures = least_squares_fit_cp_2(T_high, Cp_high - a_high)
    coeff_high = np.insert(coeff_high_temperatures, 0, a_high)
    
    # Cp_do_low = Cp_function_low(T_check, *coeff_low)
    # Cp_do_high = Cp_function_high(T_check, *coeff_high)

    # print_data(name, Cp_do_low, Cp_do_high)
    # print(Cp_do_low, Cp_do_high)

    # print_coef(name, coeff_low, coeff_high)
    
    return temperatures, cp_values, S_values, H_values, coeff_low, coeff_high
    
def generate_plot_with_original(output_dir, name, temperatures, values, T_full, fit_low, fit_high):
    T_low = T_full[((T_full <= config.max_low_temperature) & (T_full >= 100))]
    T_high = T_full[(T_full >= config.max_low_temperature) & (T_full <= config.max_high_temperature)]

    values = values[(temperatures < config.max_high_temperature)]
    temperatures = temperatures[temperatures < config.max_high_temperature]
    
    fig, ax = plt.subplots(figsize=(10, 6), dpi=150)
    ax.scatter(temperatures, values, color='#333333', label='Реальные данные', alpha=0.7, s=15,
            marker='o', edgecolors='none')
    ax.plot(T_low, fit_low, color='#1f77b4', label='Низкие температуры', linewidth=2)
    ax.plot(T_high, fit_high, color='#d62728', label='Высокие температуры', linewidth=2)
    ax.xaxis.set_major_locator(MaxNLocator(integer=True))
    ax.yaxis.set_major_locator(MaxNLocator(integer=True))
    ax.annotate('Пороговая температура', xy=(350, 450), xytext=(280, 550),
                arrowprops=dict(facecolor='black', shrink=0.05), fontsize=12)
    ax.set_xlabel('Температура (K)', fontsize=14)
    ax.set_ylabel(name, fontsize=14)
    result = output_dir.split('/')[-1]
    ax.set_title(f'Зависимость {name} от температуры {result}', fontsize=16)
    ax.legend(fontsize=12)
    ax.grid(True, linestyle='--', which='major', color='grey', alpha=0.75)
    plt.savefig(os.path.join(output_dir, f'{name}.png'), bbox_inches='tight')
    plt.close()

def process_substance(output_dir, name, filename, H_0, S_0):
    (temperatures, cp_values, S_values, H_values, coeff_low, coeff_high) = get_data(filename, name)
    T_full = np.linspace(100, config.max_high_temperature, 100)

    # print(name)
    (Cp_fit_low, Cp_fit_high, H_fit_low, H_fit_high, S_fit_low, S_fit_high) = get_properties(T_full, coeff_low, coeff_high, H_0, S_0)

    # print(((np.sum(Cp_fit_low) - np.sum(cp_values[temperatures < config.max_low_temperature]))**2) / np.size(Cp_fit_low))

    if not os.path.exists(output_dir + '/' + name):
        os.makedirs(output_dir + '/' + name)

    # T = T_full
    # T_low = T[(T <= config.max_low_temperature)]
    # T_high = T[(T >= config.max_low_temperature) & (T <= config.max_high_temperature)]
    # F_fit_low, F_fit_high = F_function_low(T_low, *coeff_low, S_0, H_0), F_function_low(T_high, *coeff_low, S_0, H_0)

    generate_plot_with_original(output_dir + '/' + name, 'Cp', temperatures, cp_values, T_full, Cp_fit_low, Cp_fit_high)
    generate_plot_with_original(output_dir + '/' + name, 'S', temperatures, S_values, T_full, S_fit_low, S_fit_high)
    generate_plot_with_original(output_dir + '/' + name, 'H', temperatures, H_values, T_full, H_fit_low, H_fit_high)

    data = []
    for i in range(len(T_full)):
        data.append({
            'Temperature': T_full[i],
            'Cp': np.concatenate((Cp_fit_low, Cp_fit_high))[i],
            'S': np.concatenate((S_fit_low, S_fit_high))[i],
            'H': np.concatenate((H_fit_low, H_fit_high))[i]
        })

    # Запись данных в CSV файл
    with open('output' + name + '.csv', mode='w', newline='') as file:
        writer = csv.DictWriter(file, fieldnames=['Temperature', 'Cp', 'S', 'H'])
        writer.writeheader()
        writer.writerows(data)

    return T_full, coeff_low, coeff_high
