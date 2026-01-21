document.onload = provjeriIdentitet();

const firebaseConfig = {
    apiKey: "AIzaSyAtxCyv2_sef_Ma-tGPgAfbyfUHeqqjuf0",
    authDomain: "petfeeder-28ccf.firebaseapp.com",
    databaseURL: "https://petfeeder-28ccf-default-rtdb.europe-west1.firebasedatabase.app",
    projectId: "petfeeder-28ccf",
    storageBucket: "petfeeder-28ccf.appspot.com",
    messagingSenderId: "308375633698",
    appId: "1:308375633698:web:e88eca13f2404990c8af7b",
    measurementId: "G-02NPFV157P"
};

firebase.initializeApp(firebaseConfig);
const uredjajdb = firebase.database().ref();

var id = -1;
document.getElementById("dodaj-uredjaj").onclick = () => {
    dodajUredjajZaKorisnika();
}
document.getElementById("promijeni-lozinku").onclick = promijeniLozinku;
document.getElementById("button-zaboravi").onclick = zaboraviWifi;
document.onload = ucitajUredjaje();

function dodajUredjaj() {
    let url = `${urlAplikacije}/Uredjaj/DodajUredjaj`;

    let obj = new Object();
    obj.ime = "x";
    fetch(url, {
        method: "POST",
        headers: {
            "Content-Type": "application/json",
        },
        body: JSON.stringify(obj)
    }).then(response => {
        if (response.status != 200) {
            porukaError("Server javlja grešku: " + response.statusText);
            return -1;
        }
        response.json().then(r => {
            id = r.uredjajID;
        })
    }).catch(error => {
        porukaError("Greška u komunikaciji sa serverom " + error.statusText);
    })
}

function dodajUredjajZaKorisnika() {
    let url = `${urlAplikacije}/KorisnikUredjaj/DodajUredjajZaKorisnika`;
    let imeUredjaja = document.getElementById("ime-uredjaja").value;
    let macUredjaja = document.getElementById("mac-uredjaja").value;

    if (!ValidirajString(imeUredjaja)) {
        porukaError("Uređaj mora imati ime!");
        return;
    }
    if (!ValidirajMac(macUredjaja)) {
        porukaError("Uredjaj mora imati validnu MAC adresu!");
        return;
    }
    let obj = new Object();
    obj.korisnikID = parseInt(sessionStorage.getItem(korisnikId));
    obj.uredjajIme = imeUredjaja;
    obj.uredjajMac = macUredjaja;
    fetch(url, {
        method: "POST",
        headers: {
            "Content-Type": "application/json",
        },
        body: JSON.stringify(obj)
    }).then(response => {
        if (response.status != 200) {
            porukaError("Server javlja grešku: " + response.statusText);
            return;
        }
        uredjajdb.child(macUredjaja).on("value", (snap) => {
            if (snap.val() == null) {
                uredjajdb.child(macUredjaja).set({
                    uredjajAktivan: -1,
                    dodajHranu: -1,
                    aktivacijeIzmijenjene: -1,
                    zaboraviWifi: -1,
                    resetovanaIzbacivanja: -1,
                    hranaIzbacena: -1
                })
            };
        })
        porukaSuccess("Uređaj je uspjesno dodat.");
        ucitajUredjaje();
        document.getElementById("ime-uredjaja").value = '';
        document.getElementById("mac-uredjaja").value = '';
    }).catch(error => {
        porukaError("Greška u komunikaciji sa serverom " + error.statusText);
    })
}

function ucitajUredjaje() {
    let url = `${urlAplikacije}/KorisnikUredjaj/GetUredjajeZaKorisnika?korisnikId=${sessionStorage.getItem('korisnikID')}`;
    fetch(url).then(response => {
        if (response.status != 200) {
            porukaError("Server javlja grešku: " + response.statusText);
            return;
        }
        response.json().then(r => {
            document.getElementById("moji-uredjaji").innerHTML = '';

            for (let i = 0; i < r.length; i++) {
                document.getElementById("moji-uredjaji").innerHTML += `
                    <div id = "${r[i].ime}" class="uredjaj" >
                    <h5>${r[i].ime}</h5>
                    <button onclick="obrisiUredjaj(${r[i].uredjajID})">Obriši</button>
                </div >
                `;
            }
        })
    }).catch(error => {
        porukaError("Greška u komunikaciji sa serverom " + error.statusText);
    })
}

function obrisiUredjaj(id) {
    let url = `${urlAplikacije}/KorisnikUredjaj/DeleteUredjaj?korisnikId=${sessionStorage.getItem('korisnikID')}&uredjajId=${id}`;

    fetch(url, {
        method: 'DELETE'
    }).then(response => {
        if (response.status != 200) {
            porukaError("Server javlja grešku: " + response.statusText);
            return;
        }
        uredjajdb.child(sessionStorage.getItem(uredjajMac)).remove();
        porukaSuccess("Uređaj je uspjesno obrisan!");
        ucitajUredjaje();
    }).catch(error => {
        porukaError("Greška u komunikaciji sa serverom " + error.statusText);
    })
}

function promijeniLozinku() {
    let url = `${urlAplikacije}/Korisnik/PromijeniLozinku`;
    let staraLozinka = document.getElementById("stara-lozinka").value;
    let novaLozinka1 = document.getElementById("nova-lozinka1").value;
    let novaLozinka2 = document.getElementById("nova-lozinka2").value;

    let obj = new Object();
    if (!ValidirajLozinku(staraLozinka) || !ValidirajLozinku(novaLozinka1) || !ValidirajLozinku(novaLozinka2)) {
        porukaError("Lozinke nisu ispravno unešene!");
        porukaError('Lozinka mora sadržavati najmanje 8 znakova, velika i mala slova i brojeve.');
        return;
    }
    if (novaLozinka1 != novaLozinka2) {
        porukaError("Lozinke se ne slažu!");
        return;
    }
    obj.korisnikID = sessionStorage.getItem("korisnikID");
    obj.staraLozinka = encoder(staraLozinka);
    obj.novaLozinka = encoder(novaLozinka1);


    fetch(url, {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify(obj)
    }).then(response => {
        if (response.status != 200) {
            porukaError("Server javlja grešku: " + response.statusText);
            return;
        }
        porukaSuccess("Lozinka je uspješno promijenjena!");
        document.getElementById("stara-lozinka").value = '';
        document.getElementById("nova-lozinka1").value = '';
        document.getElementById("nova-lozinka2").value = '';
    }).catch(error => {
        porukaError("Greška u komunikaciji sa serverom " + error.statusText);
    })
}

function zaboraviWifi() {
    let macUredjaja = sessionStorage.getItem(uredjajMac);
    let zaboraviWifi = uredjajdb.child(macUredjaja).child("zaboraviWifi");
    zaboraviWifi.set(1);
    zaboraviWifi.on("value", (snap) => {
        if (snap.val() == 1) {
            porukaSuccess("WiFi je uspješno zaboravljen!<br>Uređaj će se restartovati kako biste postavili novi WiFi");
        }
    })
}