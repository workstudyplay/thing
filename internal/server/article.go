package server

import (
	"context"
	"fmt"
	"time"

	"github.com/samsarahq/thunder/graphql/schemabuilder"
	"github.com/samsarahq/thunder/reactive"
	"github.com/workstudyplay/thing/internal/elements/article"
)

type CreateArticleRequest struct {
	Title  string
	Author string
	Body   string
}

func (s *server) registerArticles(schema *schemabuilder.Schema) {

	obj := schema.Object("Article", article.Article{})
	obj.FieldFunc("age", func(ctx context.Context, a *article.Article) string {
		reactive.InvalidateAfter(ctx, 5*time.Second)
		return time.Since(a.CreatedAt).String()
	})

	q := schema.Query()
	q.FieldFunc("articles", func() []article.Article {
		return s.articles
	})

	m := schema.Mutation()
	m.FieldFunc("createArticle", func(args CreateArticleRequest) (article.Article, error) {
		fmt.Println("Created Article")
		p := article.Article{
			Title:     args.Title,
			Author:    args.Author,
			CreatedAt: time.Now(),
			Body:      args.Body,
		}
		return p, nil
	})
}
