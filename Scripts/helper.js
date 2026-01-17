function redirekcija(url) {
    window.open(url, "_self");
}

const korisnikId = "korisnikID";
const uredjajMac = "uredjajMAC";

function provjeriIdentitet() {
    if (sessionStorage.getItem("korisnikID") == null)
        redirekcija("../login.html");
}



function enkriptuj(unos) {
    let e = 11;
    let n = 323;
    let enkriptovano = 1;
    for (let i = e; i > 0; i--) {
        let code = unos.charCodeAt(0);
        enkriptovano *= code;
        enkriptovano %= n;
    }
    return String.fromCharCode(enkriptovano);
}

function encoder(plain) {
    let velicina = plain.length;
    let rezultat = "";
    for (let i = 0; i < velicina; i++) {
        rezultat += enkriptuj(plain[i]);
    }

    return rezultat;
}
const urlAplikacije = `/api`;