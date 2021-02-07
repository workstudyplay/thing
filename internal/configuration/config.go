package configuration

import (
	"encoding/json"
	"log"

	flag "github.com/spf13/pflag"
	"github.com/spf13/viper"
	"github.com/subosito/gotenv"
)

const (
	flagForPublicURI   = "public_uri"
	flagForHTTPPort    = "service_port"
	flagForEnvironment = "environment"
	flagForRedisURI    = "redis_uri"
	flagForPostgresURI = "postgres_uri"
)

type Config struct {
	PublicURI   string
	HTTPPort    int
	Environment string
	RedisURI    string
	PostgresURI string
}

func (c Config) String() string {
	b, err := json.Marshal(c)
	if err != nil {
		log.Fatalf("Could not marshal config to string: %v", err)
	}
	return string(b)
}

// LoadConfig reads in configuration settings from the environment.
func LoadConfig() Config {
	return LoadConfigWithFile(".env")
}

func LoadConfigWithFile(envFile string) Config {
	c := Config{
		HTTPPort: 3030,
		RedisURI: "",
	}

	flag.String(flagForPublicURI, c.PublicURI, "Publically accessible URI")
	flag.Int(flagForHTTPPort, c.HTTPPort, "HTTP port")
	flag.String(flagForEnvironment, c.Environment, "Environment")
	flag.String(flagForRedisURI, c.RedisURI, "Redis URI")
	flag.String(flagForPostgresURI, c.PostgresURI, "Postgres URI")

	flag.Parse()

	viper.BindPFlag(flagForPublicURI, flag.Lookup(flagForPublicURI))
	viper.BindPFlag(flagForHTTPPort, flag.Lookup(flagForHTTPPort))
	viper.BindPFlag(flagForEnvironment, flag.Lookup(flagForEnvironment))
	viper.BindPFlag(flagForRedisURI, flag.Lookup(flagForRedisURI))
	viper.BindPFlag(flagForPostgresURI, flag.Lookup(flagForPostgresURI))

	// Load overrides from .env
	gotenv.Load(envFile)
	viper.AutomaticEnv()

	c.PublicURI = viper.GetString(flagForPublicURI)
	c.HTTPPort = viper.GetInt(flagForHTTPPort)
	c.Environment = viper.GetString(flagForEnvironment)
	c.RedisURI = viper.GetString(flagForRedisURI)
	c.PostgresURI = viper.GetString(flagForPostgresURI)

	return c
}
