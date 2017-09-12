#include "catalogentry.h"

//CatalogEntry::CatalogEntry() {
//	energySet = false;
//	nameSet = false;
//}

CatalogEntry::CatalogEntry(double frequency, double intensity) {
	C = QString();
	FREQ = frequency;
	LGINT = intensity;
	DR = -1;
	ELO = -1.;
}

CatalogEntry::CatalogEntry(const QString &name, double frequency, double intensity, qint8 degreesOfFreedom, double lowerStateEnergy) {
	C = name;
	FREQ = frequency;
	LGINT = intensity;
	DR = degreesOfFreedom;
	ELO = lowerStateEnergy;
}

CatalogEntry::CatalogEntry(const QString &name, double frequency, double intensity) {
	C = name;
	FREQ = frequency;
	LGINT = intensity;
	DR = -1;
	ELO = -1.;
}

CatalogEntry::CatalogEntry(double frequency, double intensity, qint8 degreesOfFreedom, double lowerStateEnergy) {
	C = QString();
	FREQ = frequency;
	LGINT = intensity;
	DR = degreesOfFreedom;
	ELO = lowerStateEnergy;
}

CatalogEntry::CatalogEntry(const QString &catalogLine) {
//   FREQ, ERR, LGINT, DR,  ELO, GUP, TAG, QNFMT,  QN',  QN"
//	(F13.4,F8.4, F8.4,  I2,F10.4,  I3,  I7,    I4,  6I2,  6I2)
//	FFFFFFFF.FFFFEEE.EEEE-II.IIIIDDEEEEE.EEEEGGG+TTTTTTQQQQ112233445566112233445566
//	     262.0870  0.0011-19.2529 2 5174.7303  4  180011335 1-132 2 2   1 132 2 3
	FREQ = catalogLine.mid(0, 13).toDouble();
	LGINT = catalogLine.mid(21, 8).toDouble();
	DR = catalogLine.mid(29, 2).toInt();
	ELO = catalogLine.mid(31, 10).toDouble();
}

void CatalogEntry::setLowerStateEnergy(qint8 degreesOfFreedom, double lowerStateEnergy) {
	ELO = lowerStateEnergy;
	DR = degreesOfFreedom;
}

void CatalogEntry::setName(const QString &name) {
	C = name;
}

const QString & CatalogEntry::name() const {
	return C;
}

double CatalogEntry::frequency() const {
	return FREQ;
}

double CatalogEntry::intensity(double T = -1.) const {
	if (DR >= 0 && T > 0. && T != T0) {
		return LGINT + (0.5 * (double)DR + 1.0) * qLn(T0/T) / M_LOG10E
				 + (-(1/T-1/T0) * ELO * 100. * h * c / k) / M_LOG10E;
	}
	else {
		return LGINT;
	}
}

qint8 CatalogEntry::degreesOfFreedom() const {
	return DR;
}

double CatalogEntry::lowerStateEnergy() const {
	return ELO;
}

double CatalogEntry::frequencyMHz() const {
	return frequency();
}

double CatalogEntry::frequencyGHz() const {
	return MHz2GHz(frequency());
}

double CatalogEntry::frequencyRevCm() const {
	return MHz2revCm(frequency());
}

double CatalogEntry::frequencyNm() const {
	return MHz2nm(frequency());
}

QJsonObject CatalogEntry::toJsonObject() {
	return QJsonObject({ {"frequency", FREQ}, {"intensity", LGINT}, {"lowerstateenegry", ELO} });
}

double MHz2GHz(double frequencyMHz) {
	return frequencyMHz * 1e-3;
}

double MHz2revCm(double frequencyMHz) {
	return frequencyMHz * 1e4 / c;
}

double MHz2nm(double frequencyMHz) {
	return c / frequencyMHz * 1e3;
}

double GHz2MHz(double frequencyGHz) {
	return frequencyGHz * 1e3;
}

double GHz2revCm(double frequencyGHz) {
	return frequencyGHz * 1e7 / c;
}

double GHz2nm(double frequencyGHz) {
	return c / frequencyGHz;
}

double revCm2MHz(double frequencyRevCm) {
	return frequencyRevCm * 1e-4 * c;
}

double revCm2GHz(double frequencyRevCm) {
	return frequencyRevCm * 1e-7 * c;
}

double revCm2nm(double frequencyRevCm) {
	return 1e7 / frequencyRevCm;
}

double nm2MHz(double frequencyNm) {
	return c / frequencyNm * 1e-3;
}

double nm2GHz(double frequencyNm) {
	return c / frequencyNm;
}

double nm2revCm(double frequencyNm) {
	return 1e7 / frequencyNm;
}

double sqNmMHz2cmPerMolecule(double intensitySqNmMHz) {
	return -10. + intensitySqNmMHz - qLn(c)/M_LOG10E;
}

double cmPerMolecule2sqNmMHz(double intensityCmPerMolecule) {
	return intensityCmPerMolecule + 10. + qLn(c)/M_LOG10E;
}
