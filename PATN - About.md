**About PATN**

PATN started life way back in 1984 as a practical tool for my CSIRO research into 'pattern analysis', a field established by the botanist, Professor Bill Williams and the computer scientist, Dr Godfrey Lance at CSIRO in Canberra Australia.

The first version was written for **DOS** and contained an extremely broad range of modules and options. Experience suggested that 5% of the options were used 95% of the time. PATN was designed to confront users with an array of options to ensure that it was not used as a 'black box'. That failed as many users tried all options. "A man with one watch knows what time it is. A man with two watches is never sure".

**PATN (for Windows) v1.00** built on the knowledge of thousands of analyses. PATN examines an imported dataset and derives a suite of appropriate options. A 'standard' analysis can now be done in a few seconds by pressing a few buttons.

**PATN v3** was the first commercial release of PATN for Windows™. The key innovation in PATN v3 is the animated, interactive ordination plot display. This is the window where most interpretation occurs. This environment is the ideal way to understand your data.

As of **March, 2019**, I have placed PATN into the 'public domain'. For now, that means you can freely download the Windows installer version (v4). The source code will be placed on GitHub as soon as we get our act together. Once that is done, as per GitHub, at least developmental aspects of PATN will migrate there. I have placed PATN into the public domain as I am less able through time to support it, and develop it further, but I still have some ideas that I would like to see implemented. I will create a new page on this site that lists these ideas.

PATN v3, like it's predecessor DOS PATN, contains four basic components-

**Input and output of data and results**

PATN can import data in Excel™ or 'csv' (comma-separated-variable) format or even DOS PATN (both .prm/.dat/.clb/.rlb and .arc formats). PATN can also and input or export data tables, group compositions, ordination coordinates, dendrogram ordering and all basic statistics.

**Preparation or pre-processing**

PATN contains a range of transformation and standardization options that can be quickly applied to selected rows and columns of the Data Table. 'Visible Statistics' such as minima, means, maxima, values \> 0, sum and missing values (user selectable) are also displayed in margins of the Data Table. PATN can also rapidly display (a button press).

-   Histograms of one or more columns or rows of the Data Table
-   Scatter plots of two columns or rows of the Data Table

**Analysis**

PATN contains a broad mix of analysis utilities-

-   Association measures: Bray and Curtis, Minkowski Series, Two-Step and Kulczynski.
-   Classification: Hierarchical classification module with options Nearest Neighbour, Furthest Neighbour, Flexible UPGMA and Flexible WPGMA and a fast non-hierarchical module based on ALOC/B that can handle millions of objects.
-   Ordination: PATN uses the SSH algorithm (semi-strong hybrid multidimensional scaling). This algorithm can perform anything from pure metric to pure non-metric multidimensional scaling.
-   Networks: PATN can display the display a minimum spanning tree on the ordination plot to assist the evaluation of data structures.

**Analysis Evaluation**

PATN contains a range of options that are designed to help you evaluate the analysis

-   Two-Way Table: PATN generates a coloured coded two-way table the classification of objects and variables.
-   Box and Whisker Plots. PATN's traditional box and whisker plots can be sorted by Data Table order of variables, by Kruskal-Wallis values or alphabetically.
-   ANOSIM across all groups (intrinsic or extrinsic) or all pair-wise groups.
-   Kruskal-Wallis values are used to estimate the utility of variables to discriminate between a set of groups. The Kruskal-Wallis values are a non-parametric equivalent to an f-ratio (using ranked values).
-   Principal Component Correlation (PCC) runs multiple linear regression on selected variables fin the data Table and the ordination coordinates. The result is a vector for each variable embedded in the ordination plot (with correlations calculated).
-   Monte-Carlo Attributes in Ordination (MCAO) randomly assigns values of variables to objects in the ordination plot and runs PCC a user-selected number of times (usually 100) to evaluate the 'robustness' of the PCC results.
-   A-Priori groupings for objects can be imported and used for diagnosis in the 3d ordination plot, box and whisker plots and ANOSIM.

Once the analysis options have been selected, PATN will run the complete analysis in a few seconds.

**he 3d ordination plot**

Just about all aspects of an analysis can be visualized on the ordination plot. It is this window where you will (if you're' smart) spend most of your time while using PATN.

Features of this display include-

-   Rotate the plot in any direction automatically or manually
-   Meaningful object colours (or black and white symbols or labels or user-defined colours if you wish) or even group colors (PATN's or user-defined/a-priori)
-   Display group centroids or all objects (size of objects is user-defined)
-   Highlight differences between PATN and a-priori groups
-   Preselcted user-defined groups of objects can be displayed
-   Clicking objects will display labels and highlight the record in Data Table
-   Drag between objects to display the association value
-   Drag between group centroids to display best Kruskal-Wallis values
-   Display minimum spanning tree
-   Display data vectors from PCC
-   Display a legend of objects
-   Annotate the 3d plot with text
-   Save plot as an image or a video with the plot rotating through 360

**Other Features of PATN**

-   Extensive help with a comprehensive browse sequence (logical learning sequence)
-   Analysis can be done on either rows (objects) or columns (variables) of the Data Table.
-   Row or columns can be designated as either intrinsic (used in the analysis) or extrinsic (used only in the evaluation).
-   Extensive right mouse button functions in context for each window
-   User-selectable basic statistical values can be displayed in the margins of the Data Table. This helps spot data issues.
-   Results are readily available in separate TABS from the Data Table or via icons.
-   The 'Recipe' of analysis and evaluation steps are maintained in a time stamped log file (similar to what DOS PATN used to use). This file can be exported as a text file.
-   Dendrograms are scalable (you can even display a group dendrogram) and the fusion history is still visible in a dual panel.
-   Association matrices can be displayed, printed, exported an imported.

**System Requirements**

The faster the processor and the more memory you have in your system, the faster PATN will run and the bigger the datasets that can be analzed without heading out for a meal: See [timing](/f-a-q/) for larger datasets. The minimal configuration is-

-   Pentium 200 (!) or above with at least 512MB memory, 5mb disk space for PATN and an 800 \* 600 colour display (i.e., not a lot!) and
-   Windows™ 98, 2000, XP, Me, Vista, Windows 7, Windows 8 (i.e., anything vaguely Windows!).

"I think you will find that PATN will provide you with fresh insights into your data and the bonus is that it is easy and fun to use."

Lee Belbin
