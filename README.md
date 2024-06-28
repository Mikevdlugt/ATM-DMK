# ATM-DMK
Pinautomaat Project
Welkom bij ons Pinautomaat Project! In dit document beschrijven we wat we hebben gedaan, hoe het werkt en hoe je het kunt gebruiken. Dit is geschreven in begrijpelijke taal zodat iedereen, ook niet-technische mensen, kunnen begrijpen wat we hebben gemaakt.

Overzicht
We hebben een pinautomaat gebouwd die werkt met een tablet en een Arduino. Het systeem maakt gebruik van Java Swing voor de gebruikersinterface en communiceert met een API en server om gegevens van bankgebruikers op te halen.

Onderdelen van het Project
1. Tablet met Java Swing
Java Swing: Dit is een programmeertaal die we hebben gebruikt om de interface van de pinautomaat te maken. Het zorgt ervoor dat alles wat je op het scherm ziet er netjes uitziet en goed werkt.
Tablet: Op de tablet zie je de interface van de pinautomaat. Hier kun je interactie hebben met het systeem, zoals je pasje scannen, je pincode invoeren, en geld opnemen.
2. Arduino met hardware
Arduino: Dit is een klein stukje hardware dat we hebben gebruikt om de fysieke onderdelen van de pinautomaat te besturen. De fysieke onderdelen zitten allemaal aan de arduino vast die al deze acties dan weer doorgegeeft aan de tablet.
3. API en Server
API (Application Programming Interface): Dit is een stukje software dat ervoor zorgt dat onze pinautomaat kan communiceren met de bankserver om informatie op te halen. Bijvoorbeeld, wanneer je je pasje scant, vraagt de API om je accountinformatie bij de bank.
Server: Dit is waar alle gegevens van de bankgebruikers zijn opgeslagen. De server stuurt de benodigde informatie terug naar de pinautomaat wanneer daarom wordt gevraagd.

Hoe het Werkt
Pasje Scannen: Houd je bankpas voor de RFID-reader bij de pinautomaat. De Arduino leest de gegevens van je pasje en stuurt deze door naar de tablet.
Gegevens Ophalen: De tablet vraagt via de API de gegevens op van jouw bankrekening bij de server.
Interface: Op de tablet zie je je saldo en kun je kiezen wat je wilt doen, bijvoorbeeld geld opnemen.
Actie Uitvoeren: Naast de tablet zitten knoppen die zijn verbonden aan de arduino. Door middel van deze knoppen kan je bedragen kiezen die je wilt opnemen. De tablet stuurt deze informatie terug naar de Arduino die de fysieke actie van geld uitgeven uitvoert.
Bevestiging: De tablet toont een bevestiging van de transactie en stuurt deze gegevens ook weer vie de API zodat je afschrift ook echt gedaan is.

Auteurs
Dit project is gemaakt door Daniel Huijsdens, Kai van der Knijff, Mike van der Lugt, en Daan Meijer.


