# Instructions for Adding Figure 2 from the Paper

## Required Image

The LaTeX document references: `figure2_lift_coefficient.png`

This should be Figure 2 from Coupier et al. (2009) showing:
- X-axis: $a_2/\hat{a}_1$ (vesicle deflation ratio, 0.75 to 1.0)
- Y-axis: $3U[a_1/\hat{a}_1]$ (scaled lift velocity)
- Black squares: experimental data points
- Red line: theoretical curve

## How to Add

1. Extract Figure 2 from `ProceedingMRS.PDF`
2. Save as: `figure2_lift_coefficient.png` in the main directory
3. Recompile: `pdflatex text.tex`

## Alternative

If the figure is not available, you can:
- Comment out the figure in `text.tex` (lines ~125-131)
- Or create a placeholder using the experimental data points

The document compiles successfully without the figure, but including it provides important visual context for the lift coefficient derivation.
