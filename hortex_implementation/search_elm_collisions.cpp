double fLM(const double eta, const double gamma) {
	return eta * gamma * (1.0 - gamma);
}

double fELM(const double eta, const double gamma, const double k) {
	const double fLM_result = fLM(eta, gamma);
	const double full_result = exp2(k - fLM_result);
	
	double int_part;
	const double fractional_part = modf(full_result, &int_part);
	
	return fractional_part;
}

/*
	Was wir hier jetzt erreichen wollen ist es die Outputs zu partitionieren/bucketen.
	
	Dafür werden wir 2^{12} einzelne Dateien erstellen.
	
	Jeder 32 Bit String hat einen 12 Bit langen Prefix.
	
	Dieser Prefix identifiziert die entsprechende Datei.
	
	D.h. eine Datei ist dafür zuständig alle Outputs zu speichern,
	die den selben 12 Bit langen Prefix haben.
	
	Wir wollen ja aber Output und den korrespondierenden Input haben.
	
	Deswegen wird pro ELM Berechnung ein 64 Bit String gespeichert.
	
	Die ersten 32 Bit bestehen aus dem ELM Output.
	
	Die folgenden 32 Bit bestehen aus dem Input.
	
	Jede Datei wird anschließend sortiert nach dem Dezimalwert der ersten 32 Bit jeden Eintrags.
	
	Wenn es Kollisionen gibt, dann müssen diese nun zwei aufeinander folgende Einträge sein,
	in denen die ersten 32 Bit gleich sind.
	
	
*/
