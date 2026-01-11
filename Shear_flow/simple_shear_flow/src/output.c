/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   output.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abalk <abalk@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/10 12:00:00 by abalk             #+#    #+#             */
/*   Updated: 2026/01/10 12:00:00 by abalk            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shear.h"

/*
** Opens output file for writing CSV data.
** Returns 1 on success, 0 on failure.
*/
int	open_output_file(FILE **fp, const char *filename)
{
	*fp = fopen(filename, "w");
	if (*fp == NULL)
	{
		fprintf(stderr, "Error: Cannot open file %s\n", filename);
		return (0);
	}
	return (1);
}

/*
** Writes CSV header line to output file.
*/
void	write_csv_header(FILE *fp)
{
	fprintf(fp, "time,position_um,velocity_um_s,shear_rate,y_normalized\n");
}

/*
** Writes a single data row to CSV file.
**
** @param fp: file pointer
** @param t: time (s)
** @param y: position (m)
** @param v: velocity (m/s)
** @param gamma: shear rate (s^-1)
*/
void	write_csv_row(FILE *fp, double t, double y, double v, double gamma)
{
	double	y_norm;

	y_norm = y / CHANNEL_WIDTH;
	fprintf(fp, "%.3f,%.6f,%.6e,%.6f,%.6f\n",
		t, y * 1e6, v * 1e6, gamma, y_norm);
}

/*
** Closes output file safely.
*/
void	close_output_file(FILE *fp)
{
	if (fp != NULL)
		fclose(fp);
}
