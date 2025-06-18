const firebaseConfig = {
    apiKey: "AIzaSyAtxCyv2_sef_Ma-tGPgAfbyfUHeqqjuf",
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
let mac = null;

document.onload = provjeriIdentitet();
let ucitaneAktivacije;
let kontrolaAktivacijeDiv = document.getElementById("kontrola-aktivacije-div");

function logout() {
    redirekcija("login.html");
    sessionStorage.removeItem(korisnikId);
    sessionStorage.removeItem(uredjajMac);
}
document.onload = pripremiComboBox();

function postaviHranilicu() {
    mac = document.getElementById("uredjaji").value;
    let index = document.getElementById("uredjaji").selectedIndex;
    document.getElementById("ime-uredjaja").innerHTML = document.getElementById("uredjaji").options[index].text;
    sessionStorage.setItem(uredjajMac, mac);

    uredjajdb.child(mac).child("uredjajAktivan").on("value", (snap) => {
        document.getElementById("upali-ugasi").innerHTML = (parseInt(snap.val()) == 1) ? "Ugasi" : "Upali"
        document.getElementById("upali-ugasi").style.backgroundColor = (parseInt(snap.val()) == 1) ? "#ff0000" : "#00ff00"
    })
    uredjajdb.child(mac).child("upaljenSenzor").on("value", (snap) => {
        document.getElementById("upali-ugasi-senzor").innerHTML = (parseInt(snap.val()) == 1) ? "Ugasi senzor" : "Upali senzor"
        document.getElementById("upali-ugasi-senzor").style.backgroundColor = (parseInt(snap.val()) == 1) ? "#ff0000" : "#00ff00"
    })
    getPodatke(mac);

}
document.getElementById("upali-ugasi").onclick = () => {
    let uredjajAktivan = uredjajdb.child(mac).child("uredjajAktivan");
    (document.getElementById("upali-ugasi").innerHTML == "Upali") ? uredjajAktivan.set(1) : uredjajAktivan.set(-1)
}
document.getElementById("upali-ugasi-senzor").onclick = () => {
    let upaljenSenzor = uredjajdb.child(mac).child("upaljenSenzor");
    (document.getElementById("upali-ugasi-senzor").innerHTML == "Upali senzor") ? upaljenSenzor.set(1) : upaljenSenzor.set(-1)
}
document.getElementById("dodaj-hranu").onclick = () => {
    let dodajHranu = uredjajdb.child(mac).child("dodajHranu");
    dodajHranu.set(1);
    porukaSuccess("Hrana je dodata!");
}


document.getElementById("dnevno-puta").oninput = () => {
    if (document.getElementById("dnevno-puta").value == 0)
        ucitaneAktivacije = false;
}
document.getElementById("dnevno-puta").oninput = ucitajAktivacije;
document.getElementById("spremi").onclick = spremiAktivacije;


document.getElementById("uredjaji").onchange = () => {
    ucitaneAktivacije = false;
    postaviHranilicu();
    pripremiKontrole();
    ucitajAktivacije();
}

function pripremiComboBox() {
    var url = `${urlAplikacije}/KorisnikUredjaj/GetUredjajeZaKorisnika?korisnikId=${sessionStorage.getItem(korisnikId)}`;

    fetch(url).then(response => {
        if (response.status != 200) {
            porukaError("Server javlja grešku " + response.statusText);
        }
        response.json().then(r => {
            document.getElementById("uredjaji").innerHTML = '';
            if (r.length == 0)
                document.getElementById("uredjaji").innerHTML += `<option value="-1" selected="selected">Nemate uređaja</option>`;
            else {
                for (let i = 0; i < r.length; i++) {
                    if (i == 0)
                        document.getElementById("uredjaji").innerHTML += `<option selected value="${r[i].mac}">${r[i].ime}</option>`;
                    else
                        document.getElementById("uredjaji").innerHTML += `<option value="${r[i].mac}">${r[i].ime}</option>`;
                }
                postaviHranilicu();
                ucitajAktivacije();
            }
        })
    }).catch(err => {
        porukaError("Greška u komunikaciji sa serverom " + err.statusText);
    });
}


function pripremiKontrole() {
    let dnevnoPuta = document.getElementById("dnevno-puta").value;

    for (let i = 0; i < dnevnoPuta; i++) {
        kontrolaAktivacijeDiv.innerHTML += `
            <div id="aktivacija${i} "class="aktivacija">
            <h5>Vrijeme:</h5>
            <input id="vrijeme${i}" type="time">
            </div>
        `;
    }
}

function spremiAktivacije() {
    let urlDelete = `${urlAplikacije}/Aktivacija/DeleteAktivacijeZaUredjaj?korisnikId=${parseInt(sessionStorage.getItem(korisnikId))}&uredjajMac=${document.getElementById("uredjaji").value}`;


    fetch(urlDelete, {
        method: "DELETE"
    }).then(response => {
        if (response.status != 200) {
            porukaError("Server javlja grešku " + response.statusText);
            return;
        }
    }).catch(err => {
        porukaError("Greška u komunikaciji sa serverom " + err.statusText);
        return;
    });

    let dnevnoPuta = document.getElementById("dnevno-puta").value;

    if (dnevnoPuta == 0) {
        return;
    }
    let aktivacije = new Array();

    for (let i = 0; i < dnevnoPuta; i++) {
        let date = document.getElementById(`vrijeme${i}`).value;
        if (date == "") {
            porukaError("Morate unijeti validno vrijeme!");
            return;
        }
        let dateOdvojen = date.split(":");
        let sati = dateOdvojen[0];
        let minute = dateOdvojen[1];
        aktivacije[i] = {
            korisnikID: parseInt(sessionStorage.getItem(korisnikId)),
            uredjajMac: document.getElementById("uredjaji").value,
            vrijeme: new Date(1970, 1, 1, parseInt(sati), parseInt(minute) + 60).toJSON()
        };

    }

    let url = `${urlAplikacije}/Aktivacija/SpremiAktivacije`;
    fetch(url, {
        method: "POST",
        headers: {
            "Content-Type": "application/json",
        },
        body: JSON.stringify(aktivacije)
    }).then(response => {
        if (response.status != 200) {
            porukaError("Server javlja grešku " + response.statusText);
            return;
        }
        porukaSuccess("Aktivacije su uspješno spremljene.");
        uredjajdb.child(mac).child("aktivacijeIzmijenjene").set(1);
    }).catch(err => {
        porukaError("Greška u komunikaciji sa serverom " + err.statusText);
    });
}

function ucitajAktivacije() {
    if (document.getElementById("uredjaji").value == "-1") return;
    let dnevnoPuta = document.getElementById("dnevno-puta").value;

    let url = `${urlAplikacije}/Aktivacija/GetAktivacijeZaUredjaj?korisnikId=${sessionStorage.getItem(korisnikId)}&uredjajMac=${sessionStorage.getItem(uredjajMac)}`;

    fetch(url).then(response => {
        if (response.status != 200) {
            porukaError("Server javlja grešku " + response.statusText);
        }
        response.json().then(r => {
            kontrolaAktivacijeDiv.innerHTML = '';
            if (!ucitaneAktivacije) {
                document.getElementById("dnevno-puta").value = r.count;
                dnevnoPuta = r.count;
            }
            pripremiKontrole();

            for (let i = 0; i < dnevnoPuta; i++) {
                let date = new Date(r.aktivacije[i]?.vrijeme).toTimeString();
                if (date != undefined) {
                    let dateOdvojen = date.split(":");
                    let sati = dateOdvojen[0];
                    let minute = dateOdvojen[1];
                    document.getElementById(`vrijeme${i}`).value = `${sati}:${minute}`;
                }
            };
            ucitaneAktivacije = true;
        })
    }).catch(err => {
        porukaError("Greška u komunikaciji sa serverom " + err.statusText);
    });
}

function getPodatke(mac) {
    let url = `${urlAplikacije}/Uredjaj/GetPodatke?mac=${mac}`;
    fetch(url).then(res => {
        if (res.status == 200) {
            res.json().then(r => {

                document.getElementById("stanje-hranilice").innerHTML = r.izbacivanja;

                if (r.imaoObrokVrijeme == null) {
                    document.getElementById("historija-div").style.display = "none";
                }
                else {
                    document.getElementById("historija-div").style.display = "block";
                    let value = new Date(r.imaoObrokVrijeme);
                    let datum = `${value.getDate()}.${value.getMonth()}.${value.getFullYear()}`
                    let sati = `${value.getHours()}:${value.getMinutes()}:${value.getSeconds()}`

                    document.getElementById("historija-datum").innerHTML = datum;
                    document.getElementById("historija-vrijeme").innerHTML = sati;

                    value = new Date(r.posljednjiUpdateVrijeme);
                    datum = `${value.getDate()}.${value.getMonth()}.${value.getFullYear()}`
                    sati = `${value.getHours()}:${value.getMinutes()}:${value.getSeconds()}`

                    document.getElementById("posljednji-update-datum").innerHTML = datum;
                    document.getElementById("posljednji-update-vrijeme").innerHTML = sati;
                }
            })
        }
        else {
            porukaError(res.statusText);
        }
    }).catch(err => {
        porukaError(err.statusText);
    })
}