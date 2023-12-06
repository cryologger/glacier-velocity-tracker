#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sun Sep 10 11:34:40 2023

@author: adam
"""

# -----------------------------------------------------------------------------
# Load librarires
# -----------------------------------------------------------------------------
 
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.ticker as mticker
import matplotlib.dates as mdates
import cartopy.crs as ccrs
import cartopy.feature as cfeature
import pyproj
import seaborn as sns
from cartopy.mpl.ticker import (LongitudeFormatter, 
                                LatitudeFormatter,
                                LongitudeLocator,
                                LatitudeLocator)

# -----------------------------------------------------------------------------
# Plotting attributes
# -----------------------------------------------------------------------------

# Seaborn configuration
sns.set_theme(style="ticks")
sns.set_context("talk") # talk, paper, poster

# Set colour palette
sns.set_palette("colorblind")

# Set colour palette
sns.set_palette("turbo")

# Graph attributes
lw = 1
interval = 30
date_format = "%Y-%m-%d"

# Figure DPI
dpi = 300

# -----------------------------------------------------------------------------
# Mapping attributes
# -----------------------------------------------------------------------------

# Add Natural Earth coastline
coast = cfeature.NaturalEarthFeature("physical", "land", "10m",
                                     edgecolor="black",
                                     facecolor="lightgray",
                                     lw=0.75)

# Add Natural Earth coastline
coastline = cfeature.NaturalEarthFeature("physical", "coastline", "50m",
                                         edgecolor="black",
                                         facecolor="none",
                                         lw=0.75)

# -----------------------------------------------------------------------------
# Folder paths
# -----------------------------------------------------------------------------

# Data directory
path = "/Users/adam/Documents/GitHub/cryologger-glacier-velocity-tracker/Software/Python/"

# Figure directory
path_figures = "/Users/adam/Documents/GitHub/cryologger-glacier-velocity-tracker/Software/Python/Figures/"

# -----------------------------------------------------------------------------
# Load data
# -----------------------------------------------------------------------------

# Load data
df1 = pd.read_csv(path + 'debug_gvms_1.csv', index_col=False) # ??
df2 = pd.read_csv(path + 'debug_gvms_2.csv', index_col=False) # ??
df3 = pd.read_csv(path + 'debug_gvms_3.csv', index_col=False) # SE2

# -----------------------------------------------------------------------------
# Prepare data
# -----------------------------------------------------------------------------

df1['datetime'] = pd.to_datetime(df1['datetime'].astype(str), format='%Y-%m-%d %H:%M:%S')
df2['datetime'] = pd.to_datetime(df2['datetime'].astype(str), format='%Y-%m-%d %H:%M:%S')
df3['datetime'] = pd.to_datetime(df3['datetime'].astype(str), format='%Y-%m-%d %H:%M:%S')

# Convert bytes written to MB
df1['data_written'] = df1['bytesWritten'] / (1024 * 1024)
df2['data_written'] = df2['bytesWritten'] / (1024 * 1024)
df3['data_written'] = df3['bytesWritten'] / (1024 * 1024)


df1['bytesWritten_delta'] = df1['bytesWritten'] - df2['bytesWritten']


# -----------------------------------------------------------------------------
# Plot data
# -----------------------------------------------------------------------------

# File sizes
fig, ax = plt.subplots(figsize=(10,5))
ax.grid(ls="dotted")
sns.lineplot(x="datetime", y="data_written", data=df1, errorbar=None, lw=lw, label="Belcher Lower")
sns.lineplot(x="datetime", y="data_written", data=df2, errorbar=None, lw=lw, label="Belcher Upper")
sns.lineplot(x="datetime", y="data_written", data=df3, errorbar=None, lw=lw, label="SE2")
ax.set(xlabel=None, ylabel="File size (MB)")
plt.xticks(rotation=45, horizontalalignment="right")
ax.xaxis.set_major_formatter(mdates.DateFormatter(date_format))
ax.xaxis.set_major_locator(mdates.MonthLocator(interval=interval)) 
ax.legend(loc="center left", bbox_to_anchor=(1.0, 0.5), title="Station")
fig.savefig(path_figures + "file_size.png", dpi=dpi, transparent=False, bbox_inches="tight")

# Max buffer size
fig, ax = plt.subplots(figsize=(10,5))
ax.grid(ls="dotted")
sns.lineplot(x="datetime", y="maxBufferBytes", data=df1, errorbar=None, lw=lw, label="Belcher Lower")
sns.lineplot(x="datetime", y="maxBufferBytes", data=df2, errorbar=None, lw=lw, label="Belcher Upper")
sns.lineplot(x="datetime", y="maxBufferBytes", data=df3, errorbar=None, lw=lw, label="SE2")
ax.set(xlabel=None, ylabel="Max Buffer Size (bytes)")
plt.xticks(rotation=45, horizontalalignment="right")
ax.xaxis.set_major_formatter(mdates.DateFormatter(date_format))
ax.xaxis.set_major_locator(mdates.MonthLocator(interval=interval)) 
ax.legend(loc="center left", bbox_to_anchor=(1.0, 0.5), title="Station")
fig.savefig(path_figures + "max_buffer_size.png", dpi=dpi, transparent=False, bbox_inches="tight")

# Timer microSD
fig, ax = plt.subplots(figsize=(10,5))
ax.grid(ls="dotted")
sns.lineplot(x="datetime", y="timer_microsd", data=df1, errorbar=None, lw=lw, label="Belcher Lower")
sns.lineplot(x="datetime", y="timer_microsd", data=df2, errorbar=None, lw=lw, label="Belcher Upper")
sns.lineplot(x="datetime", y="timer_microsd", data=df3, errorbar=None, lw=lw, label="SE2")
ax.set(xlabel=None, ylabel="Timer microSD (ms)")
plt.xticks(rotation=45, horizontalalignment="right")
ax.xaxis.set_major_formatter(mdates.DateFormatter(date_format))
ax.xaxis.set_major_locator(mdates.MonthLocator(interval=interval)) 
ax.legend(loc="center left", bbox_to_anchor=(1.0, 0.5), title="Station")
fig.savefig(path_figures + "timer_micro_sd.png", dpi=dpi, transparent=False, bbox_inches="tight")


# Timer GNSS
fig, ax = plt.subplots(figsize=(10,5))
ax.grid(ls="dotted")
sns.lineplot(x="datetime", y="timer_gnss", data=df1, errorbar=None, lw=lw, label="Belcher Lower")
sns.lineplot(x="datetime", y="timer_gnss", data=df2, errorbar=None, lw=lw, label="Belcher Upper")
sns.lineplot(x="datetime", y="timer_gnss", data=df3, errorbar=None, lw=lw, label="SE2")
ax.set(xlabel=None, ylabel="Timer GNSS (ms)")
plt.xticks(rotation=45, horizontalalignment="right")
ax.xaxis.set_major_formatter(mdates.DateFormatter(date_format))
ax.xaxis.set_major_locator(mdates.MonthLocator(interval=interval)) 
ax.legend(loc="center left", bbox_to_anchor=(1.0, 0.5), title="Station")
fig.savefig(path_figures + "timer_gnss.png", dpi=dpi, transparent=False, bbox_inches="tight")


# Timer sync RTC
fig, ax = plt.subplots(figsize=(10,5))
ax.grid(ls="dotted")
sns.lineplot(x="datetime", y="timer_syncRtc", data=df1, errorbar=None, lw=lw, label="Belcher Lower")
sns.lineplot(x="datetime", y="timer_syncRtc", data=df2, errorbar=None, lw=lw, label="Belcher Upper")
sns.lineplot(x="datetime", y="timer_syncRtc", data=df3, errorbar=None, lw=lw, label="SE2")
ax.set(xlabel=None, ylabel="Timer RTC (ms)")
plt.xticks(rotation=45, horizontalalignment="right")
ax.xaxis.set_major_formatter(mdates.DateFormatter(date_format))
ax.xaxis.set_major_locator(mdates.MonthLocator(interval=interval)) 
ax.legend(loc="center left", bbox_to_anchor=(1.0, 0.5), title="Station")
fig.savefig(path_figures + "timer_sync_rtc.png", dpi=dpi, transparent=False, bbox_inches="tight")

# Timer log GNSS
fig, ax = plt.subplots(figsize=(10,5))
ax.grid(ls="dotted")
sns.lineplot(x="datetime", y="timer_logGnss", data=df1, errorbar=None, lw=lw, label="Belcher Lower")
sns.lineplot(x="datetime", y="timer_logGnss", data=df2, errorbar=None, lw=lw, label="Belcher Upper")
sns.lineplot(x="datetime", y="timer_logGnss", data=df3, errorbar=None, lw=lw, label="SE2")
ax.set(xlabel=None, ylabel="Timer Log GNSS (ms)")
plt.xticks(rotation=45, horizontalalignment="right")
ax.xaxis.set_major_formatter(mdates.DateFormatter(date_format))
ax.xaxis.set_major_locator(mdates.MonthLocator(interval=interval)) 
ax.legend(loc="center left", bbox_to_anchor=(1.0, 0.5), title="Station")
fig.savefig(path_figures + "timer_log_gnss.png", dpi=dpi, transparent=False, bbox_inches="tight")


fig, ax = plt.subplots(figsize=(10,5))
ax.set(xlabel='Date', ylabel='Logfile (MB)')
ax.grid(ls='dotted')
sns.lineplot(x='datetime', y='bytesWritten_delta', data=df1, ci=None)
plt.xticks(rotation=45, horizontalalignment='right')
ax.xaxis.set_major_formatter(mdates.DateFormatter(date_format))
ax.xaxis.set_major_locator(mdates.MonthLocator(interval=1)) 
sns.despine()

fig.savefig('/Users/adam/Downloads/size.png', dpi=dpi, transparent=False, bbox_inches='tight')




# -----------------------------------------------------------------------------
# Load data for deployments
# -----------------------------------------------------------------------------

# Load data
df = pd.read_csv(path + "cryologger_gvt_metadata.csv", index_col=False)

# Subset by visit
df = df[df["visit"].isin(["deployment"])]

# Convert to datetime
df["datetime"] = pd.to_datetime(df["date"].astype(str))

# Year
df["year"] = df["datetime"].dt.year
df["year"] = df["year"].astype(str)

# -----------------------------------------------------------------------------
# Map global deployments
# -----------------------------------------------------------------------------

# Set colour palette
sns.set_palette("turbo", 3)

# Plot map
plt.figure(figsize=(10,10))
#ax = plt.axes(projection=ccrs.Orthographic(df["longitude"].median(), df["latitude"].median())) # Centre of extents
ax = plt.axes(projection=ccrs.Orthographic(-60, 80)) # Centre of extents

ax.set_global()
ax.add_feature(cfeature.OCEAN, zorder=0)
ax.add_feature(cfeature.LAND, zorder=0, )
ax.add_feature(coastline)

gl = ax.gridlines(crs=ccrs.PlateCarree(), draw_labels=True,
                  color="black", alpha=0.25, linestyle="dotted",
                  x_inline=False, y_inline=False)
gl.top_labels = False
gl.right_labels = False
gl.rotate_labels = False
gl.xlocator = mticker.FixedLocator(np.arange(-180,180,30))
gl.ylocator = mticker.FixedLocator(np.arange(10,90,15))
gl.xformatter = LongitudeFormatter()
gl.yformatter = LatitudeFormatter()
gl.xpadding=10
sns.scatterplot(x="longitude", y="latitude", hue="year", style="year",
                markers=True, 
                data=df, zorder=10, 
                s=400, linewidth=1, edgecolor="black", legend="full",
                transform=ccrs.PlateCarree())
ax.legend(loc=4, title="Year", framealpha=1)

# Add scale bar
#scale_bar(ax, 100, (0.8, 0.025), 3)

# Save figure
plt.savefig(path_figures + "cryologger_gvt_deployments8.png", dpi=dpi, transparent=False, bbox_inches="tight")

           

# -----------------------------------------------------------------------------
# Map Yukon deployments
# -----------------------------------------------------------------------------

# Set extents
x1 = -80
x2 = -57
y1 = 65
y2 = 79.5

# Plot map
plt.figure(figsize=(10,10))
#ax = plt.axes(projection=ccrs.Orthographic(df["longitude"].median(), df["latitude"].median())) # Centre of extents
ax = plt.axes(projection=ccrs.Orthographic(-90, 70)) # Centre of extents

ax.set_extent([x1, x2, y1, y2])
#ax.set_global()
ax.add_feature(cfeature.OCEAN, zorder=0)
ax.add_feature(cfeature.LAND, zorder=0, edgecolor="black")
#ax.gridlines()

#ax.add_feature(coast)
gl = ax.gridlines(crs=ccrs.PlateCarree(), draw_labels=True,
                  color="black", alpha=0.25, linestyle="dotted",
                  x_inline=False, y_inline=False)
gl.top_labels = False
gl.right_labels = False
gl.rotate_labels = False
gl.xlocator = mticker.FixedLocator(np.arange(-180,180,30))
gl.ylocator = mticker.FixedLocator(np.arange(10,90,15))
gl.xformatter = LongitudeFormatter()
gl.yformatter = LatitudeFormatter()
gl.xpadding=10
sns.scatterplot(x="longitude", y="latitude", hue="year", style="year",
                markers=True, 
                data=df, zorder=10, 
                s=400, linewidth=1, edgecolor="black", legend="full",
                transform=ccrs.PlateCarree())
ax.legend(loc=4, title="Year", framealpha=1)

# Add scale bar
#scale_bar(ax, 100, (0.8, 0.025), 3)

# Save figure
plt.savefig(path_figures + "cryologger_gvt_deployments.png", dpi=dpi, transparent=False, bbox_inches="tight")

           



