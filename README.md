Demographic Data Analysis Application - User Guide
Version 1.0

Developed by Antropov Yuriy

Last Updated: March 24, 2025

1. Overview
The Demographic Data Analysis Application is a powerful tool designed to explore and analyze demographic data for countries worldwide. It provides an interactive world map, detailed statistics for individual countries, comparison charts for multiple countries, and a comprehensive view of global statistics with sorting capabilities. This application is ideal for users interested in geography, demographics, and data analysis.

Key features include:

Interactive world map with zoom and country selection.
Detailed statistics for individual countries, including population, area, capital, and more.
Comparison mode for visualizing population, area, and density across multiple countries.
World statistics with sortable tables and performance charts for sorting algorithms.
Data editing functionality for customizing country information.

2. Getting Started
	2.1 Launching the Application
Start the application to display the interactive world map as the main interface.
The map is centered on the screen with a vertical control panel on the left and a "Show Stats" button on the right.
	2.2 Basic Navigation
Zoom Controls: Use the slider or +/- buttons on the left panel to adjust the map scale.
Country Selection: Click on any country to highlight it and display a bubble with basic information (e.g., population).
Statistics Panel: Click "More stats" in the bubble or press the "Show Stats" button to open the statistics panel on the right.
	2.3 Initial Setup
The application requires an internet connection on the first launch to download demographic data. If the connection fails, it will attempt to load cached data from data.json.
Data is stored locally and can be refreshed manually (see Section 4.1).

3. Main Features
	3.1 Map View
Purpose: Visualize countries on an interactive world map.
Functionality:
Click a country to highlight it in yellow and display a bubble with its name and population.
Use zoom controls to focus on specific regions.
The map scale is displayed in the bubble (e.g., "Country scale: 1.33").
	3.2 Statistics Panel
Purpose: Display detailed demographic data.
Modes:
Single Country Stats: Shows detailed information for one selected country.
Comparison Mode: Displays charts comparing multiple countries.
World Stats: Provides a sortable table of all countries with performance metrics.
Access: Open via the "Show Stats" button or "More stats" in the country bubble.
	3.3 Single Country Statistics
Displayed Data:
Country name
Population (e.g., "Population: 1,380,004,385")
Capital (e.g., "Capital: New Delhi")
Region and Subregion (e.g., "Region: Asia", "Subregion: Southern Asia")
Area (e.g., "Area: 3,287,590 km²")
Currencies (e.g., "Currencies: INR")
Languages (e.g., "Languages: Hindi, English")
Timezones (e.g., "Timezones: UTC+05:30")
Phone Code (e.g., "Phone Code: +91")
Top-Level Domain (e.g., "TLD: .in")
Country Coordinates (e.g., "Country Coordinates: (20.5937, 78.9629)")
Capital Coordinates (e.g., "Capital Coordinates: (28.6139, 77.2090)")
Driving Side (e.g., "Driving Side: Left")
Flag and Coat of Arms (loaded as images from URLs)
Edit Data: Click the "Edit Data" button to modify the country's demographic information (see Section 3.6).
	3.4 Comparison Mode
Purpose: Compare demographic data across multiple countries.
Charts:
Population Comparison: Bar chart showing population sizes.
Area Comparison: Bar chart showing land areas in km².
Density Comparison: Bar chart showing population density (people/km²).
Controls: Buttons labeled "Population Compare", "Area Compare", and "Density Compare" appear when multiple countries are selected.
	3.5 World Statistics
Purpose: View and analyze data for all countries in a sortable table.
Table Columns:
Country Name
Population
Area (km²)
Sorting Options:
Sort by Population: Orders countries by population (descending).
Sort by Name: Orders countries alphabetically (ascending).
Sort by Area: Orders countries by area (descending).
Performance Charts: After sorting, a bar chart displays the execution time (in milliseconds) of three sorting algorithms:
Bubble Sort (blue)
Qt Sort (red)
std::sort (green)
	3.6 Data Editing
Purpose: Allow users to customize demographic data for a selected country.
Access: Available in the single country stats view via the "Edit Data" button.
Editable Fields: All fields listed in Section 3.3 (population, capital, etc.).
Process:
Opens a dialog window with current data.
Modify fields as needed and click "OK" to save changes.
Changes are reflected immediately in the statistics panel.

4. Navigation
	4.1 Menu Bar
File:
Refresh Data: Reloads demographic data from the internet and updates the local cache (data.json).
Exit: Closes the application.
View:
Zoom In: Increases the map scale.
Zoom Out: Decreases the map scale.
Fit to Window: Resets the map to fit the entire screen.
Compare: Toggles comparison mode on/off.
Data:
World Stats: Opens the world statistics view in the stats panel.
Help:
About: Displays developer information (Antropov Yuriy) and version details.
Documentation: Opens this user guide.
	4.2 Interface Elements
Vertical "Show Stats" Button: Located on the right side of the window. Toggles the visibility of the statistics panel.
Zoom Panel: Located on the left side of the window. Includes a slider and +/- buttons for map scaling.
Country Bubble: Appears on the map when a country is clicked, showing basic info and a "More stats" button.

5. Using Compare Mode
Enable Compare Mode:
Go to View > Compare in the menu or click the "Compare" button (if available in your UI).
The map enters selection mode.
Select Countries:
Click on countries to select them; selected countries are highlighted in red.
Click a selected country again to deselect it.
View Comparisons:
Open the stats panel with the "Show Stats" button.
Use the "Population Compare", "Area Compare", or "Density Compare" buttons to switch between bar charts.
Each country is represented by a unique color in the chart.

6. Using World Statistics
Access:
Go to Data > World Stats in the menu bar.
The stats panel switches to the world statistics view.
Sort Data:
Click "Sort by Population", "Sort by Name", or "Sort by Area" buttons to reorder the table.
The table updates instantly with the sorted data.
View Performance:
After sorting, a bar chart appears below the table, showing the time taken by Bubble Sort, Qt Sort, and std::sort.
Use this to compare algorithm efficiency.

7. Editing Country Data
Select a Country:
Click a country on the map and open its stats with "More stats" or "Show Stats".
Edit Data:
Click the "Edit Data" button in the stats panel.
A dialog window opens with the current data in editable fields.
Save Changes:
Modify any field (e.g., population, capital, etc.) and click "OK".
The stats panel updates with the new data.
Changes are stored in memory and applied to the current session (note: they may not persist after a data refresh unless saved to data.json).

8. Troubleshooting
	8.1 Data Loading Issues
Problem: No data appears on the map or in the stats panel.
Solution: Ensure an internet connection is available on first launch. If it fails, check if data.json exists in the application directory and contains valid data.
Fallback: The app uses cached data from data.json if online loading fails.
	8.2 Interface Glitches
Problem: Elements like sorting charts or buttons appear in the wrong view (e.g., sorting chart in single country stats).
Solution: This issue has been fixed in Version 2.0. If it persists, restart the application or report it to the developer.
	8.3 Missing Images
Problem: Flag or coat of arms images do not load.
Solution: Check your internet connection. If the URLs are invalid, "N/A" will be displayed instead.
	8.4 Contacting Support
For additional help, contact the developer at [insert contact info].

9. Technical Details
	9.1 System Requirements
Operating System: Windows, macOS, or Linux (Qt-compatible).
Dependencies: Qt Framework (version 5.x or 6.x), internet connection for initial data load.
Storage: Minimal space for data.json (approx. 1-2 MB).
	9.2 Data Source
Demographic data is sourced from an online API (assumed REST API, e.g., REST Countries) and cached locally in data.json.
	9.3 Architecture
Base Class: AbstractWidget (abstract class with pure virtual updateDisplay()).
Main Widget: StatsWidget (inherits from AbstractWidget, handles all statistics views).
Supporting Classes: MainWindow, DemographicsManager, MapWidget, DataEditDialog.

10. Future Enhancements
Persistent data editing with save/load options to data.json.
Additional comparison metrics (e.g., GDP, literacy rate).
Export functionality for tables and charts.
Multi-language support for the interface.
This guide reflects the current state of the application as of Version 1.0. For feedback or suggestions, please reach out to the developer. Enjoy exploring the world’s demographic data!
