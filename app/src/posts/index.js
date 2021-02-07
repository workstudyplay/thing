

export const Post = ({data}) => {
    const {id, title = '', author = '', body = ''} = data
    return (
        <div key={`post-${id}`}
            style={{
                background: 'rgba(0,0,0,.2)',
                border: '4px solid rgba(0,0,0,.4)',
                marginBottom: 10,
            }}
        >
            <h1>[{id}]{title}</h1>
            <h2>{author}</h2>
            <div>
                {body}
            </div>
        </div>
    )
}
