function volgendepagina(keuze){
    if (keuze == 0) {
        window.location.href="/index.html"; 
    }
    else if (keuze == 1) {
        window.location.href="/paslezen.html"; 
    }
    else if (keuze == 2){
        window.location.href="/pincode.html"; 
    }
    else if (keuze == 3){
        window.location.href="/keuzemenu.html"; 
    }
    else if (keuze == 4){
        window.location.href="/pinnen.html"; 
    }
    else if (keuze == 5){
        window.location.href="/standaardbiljet.html"; 
    }
    else if (keuze == 6){
        window.location.href="/bon.html"; 
    }
    else if (keuze == 7){
        window.location.href="/einde.html"; 
    }
}


var bedragKeuze = [''];
var bedragKeuze1 = "7";
var bedragKeuze2 = "0";
var bedragKeuze3 = "";
// Haal het element op waarin je de variabele wilt weergeven
var bedragElement = document.getElementById("bedrag");
// Voeg de variabele toe aan het element
bedragElement.innerHTML = "Bedrag: &pound" + bedragKeuze1 + bedragKeuze2 + bedragKeuze3;

// var naam = "John";
// var leeftijd = 25;
// var beroep = "programmeur";
// // Haal de elementen op waarin je de variabelen wilt weergeven
// var outputElement = document.getElementById("output");
// var outputElement2 = document.getElementById("output2");
// // Voeg de variabelen toe aan de elementen
// outputElement.innerHTML = "Mijn naam is " + naam + ", ik ben " + leeftijd + " jaar oud en ik werk als " + beroep + ".";
// // Voeg meer variabelen toe
// var hobby = "fietsen";
// var favorieteKleur = "blauw";
// // Voeg de extra variabelen toe aan het tweede element
// outputElement2.innerHTML = "Mijn hobby is " + hobby + " en mijn favoriete kleur is " + favorieteKleur + ".";

// function var() {
//     var outputElement = document.getElementById(outputId);
//     outputElement.innerHTML = tekst;
// }
