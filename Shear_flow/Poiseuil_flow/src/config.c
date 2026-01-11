#include "poiseuille.h"

/*
** Skips whitespace and returns pointer to first non-space character.
*/
static char	*skip_whitespace(char *str)
{
	while (*str == ' ' || *str == '\t')
		str++;
	return (str);
}

/*
** Removes trailing whitespace from a string.
*/
static void	trim_trailing(char *str)
{
	int	i;

	i = strlen(str) - 1;
	while (i >= 0 && (str[i] == ' ' || str[i] == '\t' || str[i] == '\n'))
	{
		str[i] = '\0';
		i--;
	}
}

/*
** Parses a configuration line and updates vesicle or params structure.
** Format: key = value
*/
static void	parse_line(char *line, t_vesicle *v, t_params *p)
{
	char	*key;
	char	*value;
	char	*eq;

	line = skip_whitespace(line);
	if (*line == '#' || *line == '\n' || *line == '\0')
		return ;
	eq = strchr(line, '=');
	if (!eq)
		return ;
	*eq = '\0';
	key = line;
	trim_trailing(key);
	value = skip_whitespace(eq + 1);
	trim_trailing(value);
	if (strcmp(key, "VESICLE_RADIUS") == 0)
		v->radius = atof(value) * 1e-6;
	else if (strcmp(key, "LAMBDA") == 0)
		v->lambda = atof(value);
	else if (strcmp(key, "NU") == 0)
		v->nu = atof(value);
	else if (strcmp(key, "CHANNEL_WIDTH") == 0)
		p->channel_width = atof(value) * 1e-6;
	else if (strcmp(key, "U_MAX") == 0)
		p->u_max = atof(value) * 1e-6;
	else if (strcmp(key, "DELTA") == 0)
		p->delta = atof(value);
	else if (strcmp(key, "DT") == 0)
		p->dt = atof(value);
	else if (strcmp(key, "TOTAL_TIME") == 0)
		p->total_time = atof(value);
}

/*
** Reads configuration from file and updates structures.
** Returns 1 on success, 0 on failure.
*/
int	load_config(const char *filename, t_vesicle *v, t_params *p)
{
	FILE	*fp;
	char	line[256];

	fp = fopen(filename, "r");
	if (!fp)
	{
		fprintf(stderr, "Warning: Cannot open config file %s\n", filename);
		fprintf(stderr, "Using default parameters.\n\n");
		return (0);
	}
	printf("Loading configuration from: %s\n", filename);
	while (fgets(line, sizeof(line), fp))
		parse_line(line, v, p);
	fclose(fp);
	return (1);
}

/*
** Creates a default configuration file.
*/
void	create_default_config(const char *filename)
{
	FILE	*fp;

	fp = fopen(filename, "w");
	if (!fp)
	{
		fprintf(stderr, "Error: Cannot create config file %s\n", filename);
		return ;
	}
	fprintf(fp, "# Poiseuille Flow Simulation Configuration\n");
	fprintf(fp, "# Physical parameters (units in comments)\n\n");
	fprintf(fp, "# Vesicle parameters\n");
	fprintf(fp, "VESICLE_RADIUS = 10.0    # micrometers\n");
	fprintf(fp, "LAMBDA = 1.0             # viscosity ratio\n");
	fprintf(fp, "NU = 0.95                # reduced volume\n\n");
	fprintf(fp, "# Channel parameters\n");
	fprintf(fp, "CHANNEL_WIDTH = 140.0    # micrometers\n");
	fprintf(fp, "U_MAX = 100.0            # micrometers/second\n");
	fprintf(fp, "DELTA = 1.0              # Poiseuille exponent\n\n");
	fprintf(fp, "# Simulation parameters\n");
	fprintf(fp, "DT = 0.01                # seconds\n");
	fprintf(fp, "TOTAL_TIME = 30.0        # seconds\n");
	fclose(fp);
	printf("Created default config file: %s\n", filename);
}
