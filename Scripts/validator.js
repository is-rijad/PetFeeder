function ValidirajEmail(input) {
    return /^[\w._-]+@[a-z.]+\.(com|ba)$/.test(input);
}

function ValidirajLozinku(input) {
    return (/^.*(?=.{8,})(?=.*[a-zA-Z])(?=.*\d).*$/.test(input));
}

function ValidirajString(input) {
    return (/^[\w]+$/.test(input));
}

function ValidirajMac(input) {
    return (/^([A-F0-9]){2}:([A-F0-9]){2}:([A-F0-9]){2}:([A-F0-9]){2}:([A-F0-9]){2}:([A-F0-9]){2}$/.test(input));
}