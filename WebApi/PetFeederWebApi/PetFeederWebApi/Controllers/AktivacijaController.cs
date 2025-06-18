using Microsoft.AspNetCore.Cors;
using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;
using PetFeederWebApi.Data;
using PetFeederWebApi.Models;
using PetFeederWebApi.Models.ViewModels;

namespace PetFeederWebApi.Controllers {
    [ApiController]
    [Route("[controller]/[action]")]
    public class AktivacijaController : ControllerBase
    {
        private readonly PetFeederContext _context;

        public AktivacijaController(PetFeederContext context)
        {
            _context = context;
        }

        [HttpGet]
        public IActionResult GetNextAktivaciju([FromQuery] string macAdresa)
        {
            var aktivacije = _context.Aktivacije.Include(u => u.Uredjaj)
                .Where(a => a.Uredjaj.Mac == macAdresa).ToList();
            if (aktivacije.Count == 0 )
                return NotFound();  
            aktivacije.Sort((a, b) => a.Vrijeme.CompareTo(b.Vrijeme));
            foreach (var a in aktivacije)
            {
                if (DateTime.Now.TimeOfDay <= a.Vrijeme.TimeOfDay)
                    return Ok(new ArduinoAktivacijaVM()
                    {
                        Sat = aktivacije[0].Vrijeme.Hour,
                        Minuta = aktivacije[0].Vrijeme.Minute,
                    });
            }

            return Ok(new ArduinoAktivacijaVM()
            {
                Sat = aktivacije[0].Vrijeme.Hour,
                Minuta = aktivacije[0].Vrijeme.Minute,
            });
        }

        [HttpPost]
        public IActionResult SpremiAktivacije([FromBody] AktivacijaVM[] aktivacije) {
            if (aktivacije.Length == 0)
                return NotFound("Nema nijedne aktivacije!");
            var uredjaj = _context.KorisniciUredjaji.Include(u => u.Uredjaj)
                .Where(ku => ku.KorisnikID == aktivacije[0].KorisnikID && ku.Uredjaj.Mac.Equals(aktivacije[0].UredjajMac)).FirstOrDefault();
            if (uredjaj == null)
                return BadRequest("Uredjaj ne postoji");
            foreach (var aktivacija in aktivacije) {
                _context.Aktivacije.Add(new Aktivacija()
                {
                    UredjajID = uredjaj.UredjajID,
                    Vrijeme = aktivacija.Vrijeme
                });
            }
            _context.SaveChanges();
            return Ok("Aktivacije su uspješno spremljene.");
        }

        [HttpGet]
        public IActionResult GetAktivacijeZaUredjaj([FromQuery] int korisnikId, [FromQuery] string uredjajMac) {
            var uredjaj = _context.KorisniciUredjaji.Include(u => u.Uredjaj)
                .Where(ku => ku.KorisnikID == korisnikId && ku.Uredjaj.Mac.Equals(uredjajMac)).FirstOrDefault();
            if (uredjaj == null)
                return BadRequest("Uredjaj ne postoji");
            var aktivacije = _context.Aktivacije.Where(a => a.UredjajID == uredjaj.UredjajID).ToList();
            return Ok(new {aktivacije.Count, aktivacije });
        }

        [HttpDelete]
        public IActionResult DeleteAktivacijeZaUredjaj([FromQuery] int korisnikId, [FromQuery] string uredjajMac) {
            var uredjaj = _context.KorisniciUredjaji.Include(u => u.Uredjaj)
                .Where(ku => ku.KorisnikID == korisnikId && ku.Uredjaj.Mac.Equals(uredjajMac)).FirstOrDefault();
            if (uredjaj == null)
                return BadRequest("Uredjaj ne postoji");
            var aktivacije = _context.Aktivacije.Where(a => a.UredjajID == uredjaj.UredjajID).ExecuteDelete();
            _context.SaveChanges();
            return Ok();
        }
    }
}
