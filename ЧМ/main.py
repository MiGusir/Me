import functions
import os
import config

output_dir = 'graphs'
if not os.path.exists(output_dir):
    os.makedirs(output_dir)

T_full, h2_coeffs_low, h2_coeffs_high = functions.process_substance(output_dir, 'H2', config.H2_data, config.H2_H_0, config.H2_S_0)
T_full, o2_coeffs_low, o2_coeffs_high = functions.process_substance(output_dir, 'O2', config.O2_data, config.O2_H_0, config.O2_S_0)
T_full, oh_coeffs_low, oh_coeffs_high = functions.process_substance(output_dir, 'OH', config.OH_data, config.OH_H_0, config.OH_S_0)
T_full, oh_coeffs_low, oh_coeffs_high = functions.process_substance(output_dir, 'H2O', config.H2O_data, config.H2O_H_0, config.H2O_S_0)