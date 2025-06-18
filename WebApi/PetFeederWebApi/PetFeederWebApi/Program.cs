using Microsoft.EntityFrameworkCore;
using PetFeederWebApi.Data;

var config = new ConfigurationBuilder().AddJsonFile("appsettings.json", false).AddEnvironmentVariables().Build();


var builder = WebApplication.CreateBuilder(args);

builder.Services.AddDbContext<PetFeederContext>(options =>
    options.UseSqlServer(config.GetConnectionString("PetFeederDB")));

// Add services to the container.

builder.Services.AddControllers();
// Learn more about configuring Swagger/OpenAPI at https://aka.ms/aspnetcore/swashbuckle
builder.Services.AddEndpointsApiExplorer();
builder.Services.AddSwaggerGen();

var app = builder.Build();



// Configure the HTTP request pipeline.
if (app.Environment.IsDevelopment()) {
    app.UseSwagger();
    app.UseSwaggerUI();
}

app.UseCors(
    options => options
        .SetIsOriginAllowed(x => _ = true)
        .AllowAnyMethod()
        .AllowAnyHeader()
        .AllowCredentials()
);

app.UseHttpsRedirection();

app.UseAuthorization();

app.MapControllers();

app.Run();
