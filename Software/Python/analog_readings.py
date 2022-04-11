#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Mar 31 20:40:22 2022

@author: adam
"""

import seaborn as sns
import matplotlib.pyplot as plt
from scipy import stats
from matplotlib.ticker import FormatStrFormatter

df = pd.read_csv(
    "/Users/adam/Downloads/correction.csv",
    index_col=None,
)

slope, intercept, r_value, pv, se = stats.linregress(df['voltage'],df['adc'])
 
fig, ax = plt.subplots(figsize=(10,6))
sns.regplot(x="voltage", y="adc", data=df, ci=None, label="R2 = {0:.1f}".format(r_value)).legend(loc="best")
#sns.despine()
ax.xaxis.set_major_formatter(FormatStrFormatter('%.1f'))
ax.set(xlabel='Voltage (V)', ylabel='ADC', )
ax.grid(linestyle="dotted")
plt.title("gain = 452.89 offset = -0.13")
# Save figure
fig.savefig("/Users/adam/Downloads/voltage.png", dpi=150, transparent=False, bbox_inches="tight")
 
