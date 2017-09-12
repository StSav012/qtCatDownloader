/// docs: https://www.astro.uni-koeln.de/node/477
///       https://spec.jpl.nasa.gov/ftp/pub/catalog/README
///       https://spec.jpl.nasa.gov/ftp/pub/catalog/doc/catdoc.pdf
#ifndef CATALOGENTRY_H
#define CATALOGENTRY_H

#include <QString>
#include <QtMath>
#include <QJsonObject>

class CatalogEntry
{
private:
	QString C;	// chemical name
	double FREQ;	// frequency, MHz, mandatory
	double LGINT;	// intensity, log10(nm²×MHz), mandatory
	qint8 DR;	// degrees of freedom: 0 for atoms, 2 for linear molecules, and 3 for nonlinear molecules.
	double ELO;	// lower state energy relative to the ground state, 1/cm

public:
	CatalogEntry(double frequency, double intensity);
	CatalogEntry(double frequency, double intensity, qint8 degreesOfFreedom, double lowerStateEnergy);
	CatalogEntry(const QString &name, double frequency, double intensity);
	CatalogEntry(const QString &name, double frequency, double intensity, qint8 degreesOfFreedom, double lowerStateEnergy);
	CatalogEntry(const QString &catalogLine);

	void setLowerStateEnergy(qint8 degreesOfFreedom, double lowerStateEnergy);
	void setName(const QString &name);
	const QString &name() const;
	double frequency() const;
	double intensity(double T) const;
	qint8 degreesOfFreedom() const;
	double lowerStateEnergy() const;

	double frequencyMHz() const;
	double frequencyGHz() const;
	double frequencyRevCm() const;
	double frequencyNm() const;

	QJsonObject toJsonObject();
};

static constexpr double T0 = 300.00;
static constexpr double k = 1.38064852e-23;
static constexpr double h = 6.62607004e-34;
static constexpr double c = 299792458.;

double MHz2GHz(double frequencyMHz);
double MHz2revCm(double frequencyMHz);
double MHz2nm(double frequencyMHz);
double GHz2MHz(double frequencyGHz);
double GHz2revCm(double frequencyGHz);
double GHz2nm(double frequencyGHz);
double revCm2MHz(double frequencyRevCm);
double revCm2GHz(double frequencyRevCm);
double revCm2nm(double frequencyRevCm);
double nm2MHz(double frequencyNm);
double nm2GHz(double frequencyNm);
double nm2revCm(double frequencyNm);

double sqNmMHz2cmPerMolecule(double intensitySqNmMHz);
double cmPerMolecule2sqNmMHz(double intensityCmPerMolecule);

#endif // CATALOGENTRY_H
